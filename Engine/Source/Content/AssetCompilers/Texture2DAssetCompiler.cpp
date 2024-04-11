//Header file.
#include <Content/AssetCompilers/Texture2DAssetCompiler.h>

//Core.
#include <Core/General/StaticString.h>

//Content.
#include <Content/Core/AssetHeader.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//STL.
#include <fstream>
#include <string>

//Singleton definition.
DEFINE_SINGLETON(Texture2DAssetCompiler);

/*
*	Texture 2D channel mapping class definition.
*/
class Texture2DChannelMapping final
{

public:

	//Enumeration covering all files.
	enum class File : uint8
	{
		FILE_1,
		FILE_2,
		FILE_3,
		FILE_4,
		DEFAULT //Special value for when no file should be used, but the default value specified.
	};

	//Enumeration covering all channels.
	enum class Channel : uint8
	{
		RED,
		GREEN,
		BLUE,
		ALPHA
	};

	//The file.
	File _File;

	//The channel.
	Channel _Channel;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE Texture2DChannelMapping(const File initial_file, const Channel initial_channel) NOEXCEPT
		:
		_File(initial_file),
		_Channel(initial_channel)
	{

	}

};

/*
*	Texture 2D parameters class definition.
*/
class Texture2DParameters final
{

public:

	//The first file path.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File1;

	//The second file path.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File2;

	//The third file path.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File3;

	//The fourth file path.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File4;

	//The channel mappings.
	StaticArray<Texture2DChannelMapping, 4> _ChannelMappings
	{
		Texture2DChannelMapping(Texture2DChannelMapping::File::FILE_1, Texture2DChannelMapping::Channel::RED),
		Texture2DChannelMapping(Texture2DChannelMapping::File::FILE_1, Texture2DChannelMapping::Channel::GREEN),
		Texture2DChannelMapping(Texture2DChannelMapping::File::FILE_1, Texture2DChannelMapping::Channel::BLUE),
		Texture2DChannelMapping(Texture2DChannelMapping::File::FILE_1, Texture2DChannelMapping::Channel::ALPHA)
	};

	//The default.
	Vector4<float32> _Default{ 0.0f, 0.0f, 0.0f, 1.0f };

	//The tint.
	Vector4<float32> _Tint{ 1.0f, 1.0f, 1.0f, 1.0f };

	//Whether or not to apply gamma correction.
	bool _ApplyGammaCorrection{ false };

	//The base mip level.
	uint8 _BaseMipLevel{ 0 };

	//The mipmap generation mode.
	MipmapGenerationMode _MipmapGenerationMode{ MipmapGenerationMode::DEFAULT };

};

/*
*	Default constructor.
*/
Texture2DAssetCompiler::Texture2DAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString Texture2DAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return Texture2DAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 Texture2DAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void Texture2DAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
{
	//Set up the compile data.
	CompileData *const RESTRICT compile_data{ new (_CompileDataAllocator.Allocate()) CompileData() };

	//Set the collection.
	compile_data->_Collection = compile_context._Collection;

	//Set the file path.
	compile_data->_FilePath = compile_context._FilePath;

	//Set the name.
	compile_data->_Name = compile_context._Name;

	//Set the compilation domain.
	compile_data->_CompilationDomain = compile_context._CompilationDomain;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(compile_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		Texture2DAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
}

#define ASYNC_LOAD (1)

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT Texture2DAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	Texture2DAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) Texture2DAsset() };

#if ASYNC_LOAD
	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		Texture2DAssetCompiler::Instance->LoadInternal(static_cast<LoadData*const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);
#else
	//Set up the load data.
	Texture2DLoadData load_data;

	load_data._StreamArchivePosition = load_context._StreamArchivePosition;
	load_data._StreamArchive = load_context._StreamArchive;
	load_data._Asset = new_asset;

	//Load!
	LoadInternal(&load_data);
#endif

	//Return the new asset!
	return new_asset;
}

/*
*	Compiles internally.
*/
void Texture2DAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("Texture2DAssetCompiler::CompileInternal");

	//Set up the parameters.
	Texture2DParameters parameters;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 8> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this a file 1 declaration?
			{
				const size_t position{ current_line.find("File1(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "File1() needs one argument!");

					parameters._File1 = arguments[0].Data();

					continue;
				}
			}

			//Is this a file 2 declaration?
			{
				const size_t position{ current_line.find("File2(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "File2() needs one argument!");

					parameters._File2 = arguments[0].Data();

					continue;
				}
			}

			//Is this a file 3 declaration?
			{
				const size_t position{ current_line.find("File3(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "File3() needs one argument!");

					parameters._File3 = arguments[0].Data();

					continue;
				}
			}

			//Is this a file 4 declaration?
			{
				const size_t position{ current_line.find("File4(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "File4() needs one argument!");

					parameters._File4 = arguments[0].Data();

					continue;
				}
			}

			//Is this a channel mapping declaration?
			{
				const size_t position{ current_line.find("ChannelMapping(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 3, "ChannelMapping() needs three arguments!");

					Texture2DChannelMapping* RESTRICT channel_mapping{ nullptr };

					if (arguments[0] == "RED")
					{
						channel_mapping = &parameters._ChannelMappings[0];
					}

					else if (arguments[0] == "GREEN")
					{
						channel_mapping = &parameters._ChannelMappings[1];
					}

					else if (arguments[0] == "BLUE")
					{
						channel_mapping = &parameters._ChannelMappings[2];
					}

					else if (arguments[0] == "ALPHA")
					{
						channel_mapping = &parameters._ChannelMappings[3];
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[0].Data());
					}

					if (arguments[1] == "FILE_1")
					{
						channel_mapping->_File = Texture2DChannelMapping::File::FILE_1;
					}

					else if (arguments[1] == "FILE_2")
					{
						channel_mapping->_File = Texture2DChannelMapping::File::FILE_2;
					}

					else if (arguments[1] == "FILE_3")
					{
						channel_mapping->_File = Texture2DChannelMapping::File::FILE_3;
					}

					else if (arguments[1] == "FILE_4")
					{
						channel_mapping->_File = Texture2DChannelMapping::File::FILE_4;
					}

					else if (arguments[1] == "DEFAULT")
					{
						channel_mapping->_File = Texture2DChannelMapping::File::DEFAULT;
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[1].Data());
					}

					if (arguments[2] == "RED")
					{
						channel_mapping->_Channel = Texture2DChannelMapping::Channel::RED;
					}

					else if (arguments[2] == "GREEN")
					{
						channel_mapping->_Channel = Texture2DChannelMapping::Channel::GREEN;
					}

					else if (arguments[2] == "BLUE")
					{
						channel_mapping->_Channel = Texture2DChannelMapping::Channel::BLUE;
					}

					else if (arguments[2] == "ALPHA")
					{
						channel_mapping->_Channel = Texture2DChannelMapping::Channel::ALPHA;
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[2].Data());
					}

					continue;
				}
			}

			//Is this a default declaration?
			{
				const size_t position{ current_line.find("Default(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 4, "Default() needs four arguments!");

					parameters._Default[0] = std::stof(arguments[0].Data());
					parameters._Default[1] = std::stof(arguments[1].Data());
					parameters._Default[2] = std::stof(arguments[2].Data());
					parameters._Default[3] = std::stof(arguments[3].Data());

					continue;
				}
			}

			//Is this an apply gamma correction declaration?
			{
				const size_t position{ current_line.find("ApplyGammaCorrection();") };

				if (position != std::string::npos)
				{
					parameters._ApplyGammaCorrection = true;

					continue;
				}
			}

			//Is this a base mip level declaration?
			{
				const size_t position{ current_line.find("BaseMipLevel(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "BaseMipLevel() needs one argument!");

					parameters._BaseMipLevel = static_cast<uint8>(std::stoul(arguments[0].Data()));

					continue;
				}
			}

			//Is this a mipmap generation mode declaration?
			{
				const size_t position{ current_line.find("MipmapGenerationMode(") };

				if (position != std::string::npos)
				{
					const uint64 number_of_arguments
					{
						TextParsingUtilities::ParseFunctionArguments
						(
							current_line.c_str(),
							current_line.length(),
							arguments.Data()
						)
					};

					ASSERT(number_of_arguments == 1, "MipmapGenerationMode() needs one argument!");

					if (arguments[0] == "NONE")
					{
						parameters._MipmapGenerationMode = MipmapGenerationMode::NONE;
					}

					else if (arguments[0] == "DEFAULT")
					{
						parameters._MipmapGenerationMode = MipmapGenerationMode::DEFAULT;
					}

					else if (arguments[0] == "MAXIMUM")
					{
						parameters._MipmapGenerationMode = MipmapGenerationMode::MAXIMUM;
					}

					else if (arguments[0] == "NORMAL_MAP")
					{
						parameters._MipmapGenerationMode = MipmapGenerationMode::NORMAL_MAP;
					}

					else if (arguments[0] == "OPACITY")
					{
						parameters._MipmapGenerationMode = MipmapGenerationMode::OPACITY;
					}

					else
					{
						ASSERT(false, "Unknown argument %s", arguments[0].Data());
					}

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

	//Gather the input textures.
	StaticArray<Texture2D<Vector4<float32>>, 4> input_textures;

	if (parameters._File1)
	{
		ASSERT(File::Exists(parameters._File1.Data()), "File %s does not exist!", parameters._File1.Data());

		switch (File::GetExtension(parameters._File1.Data()))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File1.Data(), &input_textures[0]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File1.Data(), &input_textures[0]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	if (parameters._File2)
	{
		ASSERT(File::Exists(parameters._File2.Data()), "File %s does not exist!", parameters._File2.Data());

		switch (File::GetExtension(parameters._File2.Data()))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File2.Data(), &input_textures[1]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File2.Data(), &input_textures[1]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	if (parameters._File3)
	{
		ASSERT(File::Exists(parameters._File3.Data()), "File %s does not exist!", parameters._File3.Data());

		switch (File::GetExtension(parameters._File3.Data()))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File3.Data(), &input_textures[2]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File3.Data(), &input_textures[2]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	if (parameters._File4)
	{
		ASSERT(File::Exists(parameters._File4.Data()), "File %s does not exist!", parameters._File4.Data());

		switch (File::GetExtension(parameters._File4.Data()))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File4.Data(), &input_textures[3]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File4.Data(), &input_textures[3]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//Determine the width and height.
	uint32 width{ 0 };
	uint32 height{ 0 };

	for (const Texture2D<Vector4<float32>> &input_texture : input_textures)
	{
		if (input_texture.GetWidth() > 0 && input_texture.GetHeight() > 0)
		{
			if (width == 0 && height == 0)
			{
				width = input_texture.GetWidth();
				height = input_texture.GetHeight();
			}

			else
			{
				ASSERT(width == input_texture.GetWidth(), "Mismatching width!");
				ASSERT(height == input_texture.GetHeight(), "Mismatching width!");
			}
		}
	}

	ASSERT(width > 0 && height > 0, "Couldn't determine width/height!");

	//Create the composite texture.
	Texture2D<Vector4<float32>> composite_texture{ width, height };

	for (uint32 Y{ 0 }; Y < composite_texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < composite_texture.GetWidth(); ++X)
		{
			//Calculate the texel.
			Vector4<float32> &texel{ composite_texture.At(X, Y) };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (UNDERLYING(parameters._ChannelMappings[i]._File) <= UNDERLYING(Texture2DChannelMapping::File::FILE_4))
				{
					texel[i] = input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].At(X, Y)[UNDERLYING(parameters._ChannelMappings[i]._Channel)];
				}

				else
				{
					texel[i] = parameters._Default[UNDERLYING(parameters._ChannelMappings[i]._Channel)];
				}

				texel[i] *= parameters._Tint[i];
			}
		}
	}

	//Destroy the input textures.
	for (uint8 i{ 0 }; i < 4; ++i)
	{
		input_textures[i].Destroy();
	}

	//Apply gamma correction, if desired.
	if (parameters._ApplyGammaCorrection)
	{
		for (uint32 Y{ 0 }; Y < composite_texture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < composite_texture.GetWidth(); ++X)
			{
				Vector4<float32> &texel{ composite_texture.At(X, Y) };

				for (uint8 i{ 0 }; i < 3; ++i)
				{
					texel[i] = powf(texel[i], 2.2f);
				}
			}
		}
	}

	//Calculate the average value.
	Vector4<float32> average_value;

	{
		//Calculate the average value with higher precision.
		Vector4<float64> _average_value{ 0.0, 0.0, 0.0, 0.0 };

		for (uint32 Y{ 0 }; Y < composite_texture.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < composite_texture.GetWidth(); ++X)
			{
				const Vector4<float32> &texel{ composite_texture.At(X, Y) };

				for (uint8 i{ 0 }; i < 4; ++i)
				{
					_average_value[i] += static_cast<float64>(texel[i]);
				}
			}
		}

		for (uint8 i{ 0 }; i < 4; ++i)
		{
			_average_value[i] /= static_cast<float64>(composite_texture.GetWidth() * composite_texture.GetHeight());
			average_value[i] = static_cast<float32>(_average_value[i]);
		}
	}
	
	//Generate the mip chain.
	DynamicArray<Texture2D<Vector4<float32>>> mip_chain;

	if (parameters._MipmapGenerationMode != MipmapGenerationMode::NONE)
	{
		RenderingUtilities::GenerateMipChain(composite_texture, parameters._MipmapGenerationMode, &mip_chain);
	}

	else
	{
		mip_chain.Emplace(composite_texture);
	}

	//Create the output textures.
	DynamicArray<Texture2D<Vector4<uint8>>> output_textures;

	output_textures.Upsize<true>(mip_chain.Size());

	for (uint64 mip_index{ 0 }; mip_index < output_textures.Size(); ++mip_index)
	{
		output_textures[mip_index].Initialize(mip_chain[mip_index].GetWidth(), mip_chain[mip_index].GetHeight());

		for (uint32 Y{ 0 }; Y < output_textures[mip_index].GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < output_textures[mip_index].GetWidth(); ++X)
			{
				for (uint8 texel_index{ 0 }; texel_index < 4; ++texel_index)
				{
					output_textures[mip_index].At(X, Y)[texel_index] = static_cast<uint8>(mip_chain[mip_index].At(X, Y)[texel_index] * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}
	}

	//Determine the collection directory.
	char collection_directory_path[MAXIMUM_FILE_PATH_LENGTH];

	if (compile_data->_Collection)
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION %s", GetCompiledDirectoryPath(compile_data->_CompilationDomain), compile_data->_Collection.Data());
	}

	else
	{
		sprintf_s(collection_directory_path, "%s\\COLLECTION Default", GetCompiledDirectoryPath(compile_data->_CompilationDomain));
	}

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(collection_directory_path);

	//Determine the directory path.
	char directory_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(directory_path, "%s\\Textures", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);

	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_data->_Name.Data());

	//Open the output file.
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data()};
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Write the average value.
	output_file.Write(&average_value, sizeof(Vector4<float32>));

	//Write the number of mipmap levels to the file.
	const uint8 mipmap_levels{ static_cast<uint8>(output_textures.Size() - parameters._BaseMipLevel) };
	output_file.Write(&mipmap_levels, sizeof(uint8));

	//Write the width and height of the texture to the file.
	const uint32 output_width{ output_textures[parameters._BaseMipLevel].GetWidth() };
	output_file.Write(&output_width, sizeof(uint32));
	const uint32 output_height{ output_textures[parameters._BaseMipLevel].GetHeight() };
	output_file.Write(&output_height, sizeof(uint32));

	//Write the texture data to the file.
	for (uint64 mip_index{ parameters._BaseMipLevel }; mip_index < output_textures.Size(); ++mip_index)
	{
		output_file.Write(output_textures[mip_index].Data(), sizeof(Vector4<uint8>) * output_textures[mip_index].GetWidth() * output_textures[mip_index].GetHeight());
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void Texture2DAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("Texture2DAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the average value.
	load_data->_StreamArchive->Read(&load_data->_Asset->_AverageValue, sizeof(Vector4<float32>), &stream_archive_position);

	//Read the number of mip levels.
	uint8 number_of_mip_levels;
	load_data->_StreamArchive->Read(&number_of_mip_levels, sizeof(uint8), &stream_archive_position);

	//Read the width/height.
	uint32 width;
	load_data->_StreamArchive->Read(&width, sizeof(uint32), &stream_archive_position);

	uint32 height;
	load_data->_StreamArchive->Read(&height, sizeof(uint32), &stream_archive_position);

	//Read the data.
	DynamicArray<DynamicArray<byte>> data;
	data.Reserve(number_of_mip_levels);
	uint32 final_width{ width };
	uint32 final_height{ height };

	for (uint8 mip_index{ 0 }; mip_index < number_of_mip_levels; ++mip_index)
	{
		const uint32 mip_width{ width >> mip_index };
		const uint32 mip_height{ height >> mip_index };
		const uint64 mip_size{ (width >> mip_index) * (height >> mip_index) * sizeof(Vector4<byte>) };

#if 0 //Low-resolution textures. (:
		if (mip_index < number_of_mip_levels - 1)
		{
			if (mip_width > 64 || mip_height > 64)
			{
				final_width >>= 1;
				final_height >>= 1;
				stream_archive_position += mip_size;

				continue;
			}
		}
#endif

		data.Emplace();
		data.Back().Upsize<false>(mip_size);
		load_data->_StreamArchive->Read(data.Back().Data(), mip_size, &stream_archive_position);
	}

	//Create the texture.
	{
		PROFILING_SCOPE("Texture2DAssetCompiler::Load::CreateTexture2D");

		RenderingSystem::Instance->CreateTexture2D
		(
			TextureData
			(
				TextureDataContainer
				(
					data,
					final_width,
					final_height,
					4
				),
				TextureFormat::RGBA_UINT8,
				TextureUsage::NONE,
				false
			),
			&load_data->_Asset->_Texture2DHandle
		);
	}

	//Add the texture to global render data.
	load_data->_Asset->_Index = RenderingSystem::Instance->AddTextureToGlobalRenderData(load_data->_Asset->_Texture2DHandle);

	//Create the texture 2D.
	load_data->_Asset->_Texture2D.Initialize(final_width, final_height);
	Memory::Copy(load_data->_Asset->_Texture2D.Data(), data[0].Data(), final_width * final_height * sizeof(Vector4<byte>));
}