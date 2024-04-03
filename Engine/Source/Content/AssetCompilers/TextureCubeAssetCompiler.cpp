//Header file.
#include <Content/AssetCompilers/TextureCubeAssetCompiler.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/ContentSystem.h>
#include <Systems/LogSystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/stb_image.h>

//STL.
#include <fstream>

//Singleton definition.
DEFINE_SINGLETON(TextureCubeAssetCompiler);

/*
*	Samples the given HDR texture in the given direction.
*/
FORCE_INLINE NO_DISCARD Vector4<float32> SampleHDRTexture(const Texture2D<Vector4<float32>> &texture, const Vector3<float32> &direction) NOEXCEPT
{
	//Define constants.
	constexpr Vector2<float32> INVERSE_ATAN{ 0.1591f, 0.3183f };

	//Calculate the texture coordinate.
	Vector2<float32> texture_coordinate{ CatalystBaseMath::ArcTangent(direction._Z, direction._X), CatalystBaseMath::ArcSine(direction._Y) };
	texture_coordinate *= INVERSE_ATAN;
	texture_coordinate += 0.5f;

	return texture.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE);
}

/*
*	Texture cube parameters class definition.
*/
class TextureCubeParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

	//The resolution.
	uint32 _Resolution;

};

/*
*	Default constructor.
*/
TextureCubeAssetCompiler::TextureCubeAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString TextureCubeAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return TextureCubeAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 TextureCubeAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void TextureCubeAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		TextureCubeAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
	};
	task->_Arguments = compile_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	compile_context._Tasks->Emplace(task);
}

/*
*	Loads a single asset with the given load context.
*/
NO_DISCARD Asset *const RESTRICT TextureCubeAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	TextureCubeAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) TextureCubeAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		TextureCubeAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
	};
	task->_Arguments = load_data;
	task->_ExecutableOnSameThread = true;

	//Execute the task!
	TaskSystem::Instance->ExecuteTask(Task::Priority::LOW, task);

	//Add the task to the list.
	load_context._Tasks->Emplace(task);

	//Return the new asset!
	return new_asset;
}

#define GATHER_SAMPLES (0)

/*
*	Compiles internally.
*/
void TextureCubeAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	//Define constants.
	constexpr uint8 MIP_LEVELS{ 9 };
	constexpr uint32 NUMBER_OF_SAMPLES{ 1024 };

	PROFILING_SCOPE("TextureCubeAssetCompiler::CompileInternal");

	//Set up the parameters.
	TextureCubeParameters parameters;

	//Set some reasonable defaults.
	parameters._Resolution = 1'024;

	//Open the input file.
	std::ifstream input_file{ compile_data->_FilePath.Data() };

	//Iterate over the lines and fill in the parameters.
	{
		StaticArray<DynamicString, 8> arguments;
		std::string current_line;

		while (std::getline(input_file, current_line))
		{
			//Is this a file declaration?
			{
				const size_t position{ current_line.find("File(") };

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

					ASSERT(number_of_arguments == 1, "File() needs one argument!");

					parameters._File = arguments[0].Data();

					continue;
				}
			}

			//Is this a resolution declaration?
			{
				const size_t position{ current_line.find("Resolution(") };

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

					ASSERT(number_of_arguments == 1, "Resolution() needs one argument!");

					parameters._Resolution = std::stoul(arguments[0].Data());

					continue;
				}
			}

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

	//Set up the HDR texture.
	Texture2D<Vector4<float32>> hdr_texture;

	{
		//Load the HDR texture.
		int32 width, height, number_of_channels;
		float32* const RESTRICT data{ stbi_loadf(parameters._File.Data(), &width, &height, &number_of_channels, STBI_rgb_alpha) };

		//Initialize the HDR texture.
		hdr_texture.Initialize(static_cast<uint32>(width), static_cast<uint32>(height));

		//Copy the data into the cpu texture.
		Memory::Copy(hdr_texture.Data(), data, width * height * sizeof(Vector4<float32>));

		//Free the data, not needed anymore.
		stbi_image_free(data);
	}

	//Create the mip chain.
	StaticArray<TextureCube, MIP_LEVELS> mip_chain;

	for (uint8 mip_level{ 0 }; mip_level < MIP_LEVELS; ++mip_level)
	{
		//Calculate the mip resolution.
		const uint32 mip_resolution{ parameters._Resolution >> mip_level };

		//Initialize the mip texture.
		mip_chain[mip_level].Initialize(mip_resolution);

#if GATHER_SAMPLES
		//Calculate the diffuse weight.
		const float32 diffuse_weight{ static_cast<float32>(mip_level) / static_cast<float32>(MIP_LEVELS - 1) };

		//Calculate the number of samples.
		const uint32 number_of_samples{ CatalystBaseMath::Maximum<uint32>(static_cast<uint32>(static_cast<float32>(NUMBER_OF_SAMPLES) * diffuse_weight), 1) };

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			for (uint32 Y{ 0 }; Y < mip_resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < mip_resolution; ++X)
				{
					//Calculate the normalized coordintace.
					const Vector2<float32> normalized_coordinate
					{
						(static_cast<float32>(X) + 0.5f) / static_cast<float32>(mip_resolution),
						(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(mip_resolution)
					};

					//Calculate the base direction
					Vector3<float32> base_direction;

					switch (face_index)
					{
						case 0: base_direction = Vector3<float32>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._X)); break; //Front.
						case 1: base_direction = Vector3<float32>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X)); break; //Back.
						case 2: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._Y)); break; //Up.
						case 3: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y)); break; //Down.
						case 4: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), -1.0f); break; //Right.
						case 5: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._X), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), 1.0f); break; //Left.
						default: CRASH(); break;
					}

					base_direction.Normalize();

					//Accumulate samples.
					Vector4<float32> accumulated{ 0.0f, 0.0f, 0.0f, 0.0f };
					float32 accumulated_weight{ 0.0f };

					for (uint32 sample_index{ 0 }; sample_index < number_of_samples; ++sample_index)
					{
						//Calculate the sample direction.
						Vector3<float32> sample_direction;

						{
							const Vector2<float32> coordinate{ CatalystRandomMath::RandomFloat() , CatalystRandomMath::RandomFloat() };

							const float32 phi{ coordinate._Y * 2.0f * CatalystBaseMathConstants::PI };
							const float32 cos_theta{ 1.0f - coordinate._X };
							const float32 sin_theta{ CatalystBaseMath::SquareRoot(1.0f - cos_theta * cos_theta) };

							sample_direction =  Vector3<float32>(CatalystBaseMath::Cosine(phi) * sin_theta, CatalystBaseMath::Sine(phi) * sin_theta, cos_theta);
						}

						//Flip the sample direction, if necessary.
						if (Vector3<float32>::DotProduct(base_direction, sample_direction) < 0.0f)
						{
							sample_direction *= -1.0f;
						}

						//Modify the sample direction based on how diffuse it is.
						sample_direction = Vector3<float32>::Normalize(CatalystBaseMath::LinearlyInterpolate(base_direction, sample_direction, diffuse_weight));
					
						//Calculate the sample weight.
						const float32 sample_weight{ CatalystBaseMath::Maximum<float32>(Vector3<float32>::DotProduct(base_direction, sample_direction), 0.0f) };

						//Sample.
						Vector4<float32> sample;

						/*
						if (mip_level > 0)
						{
							sample = mip_chain[mip_level - 1].Sample(sample_direction);
						}

						else
						*/
						{
							sample = SampleHDRTexture(hdr_texture, sample_direction);
						}

						//Accumulate!
						accumulated += sample * sample_weight;
						accumulated_weight += sample_weight;
					}

					//Normalize.
					accumulated /= accumulated_weight;

					mip_chain[mip_level].At(face_index, X, Y) = accumulated;
				}
			}
		}
#else
		//Constants.
		constexpr float32 GAUSSIAN_WEIGHTS[3][3]
		{
			{ 0.07f, 0.12f, 0.07f },
			{ 0.12f, 0.19f, 0.12f },
			{ 0.07f, 0.12f, 0.07f }
		};

		//Calculate the sample delta.
		const Vector2<float32> sample_delta{ 1.0f / static_cast<float32>(mip_resolution * 2), 1.0f / static_cast<float32>(mip_resolution * 2) };

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			for (uint32 Y{ 0 }; Y < mip_resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < mip_resolution; ++X)
				{
					if (mip_level > 0)
					{
						//Cache the previous texture.
						const Texture2D<Vector4<float32>> &previous_texture{ mip_chain[mip_level - 1].Face(face_index) };

						//Calculate the normalized coordintace.
						const Vector2<float32> normalized_coordinate
						{
							(static_cast<float32>(X) + 0.5f) / static_cast<float32>(mip_resolution),
							(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(mip_resolution)
						};

						Vector4<float32> total{ 0.0f, 0.0f, 0.0f, 0.0f };

						for (int32 _Y{ -1 }; _Y <= 1; ++_Y)
						{
							for (int32 _X{ -1 }; _X <= 1; ++_X)
							{
								total += previous_texture.Sample(normalized_coordinate + Vector2<float32>(static_cast<float32>(_X), -static_cast<float32>(_Y)) * sample_delta, AddressMode::CLAMP_TO_EDGE) * GAUSSIAN_WEIGHTS[_X + 1][_Y + 1];
							}
						}

						mip_chain[mip_level].At(face_index, X, Y) = total;
					}
					
					else
					{
						//Calculate the normalized coordintace.
						const Vector2<float32> normalized_coordinate
						{
							(static_cast<float32>(X) + 0.5f) / static_cast<float32>(mip_resolution),
							(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(mip_resolution)
						};

						//Calculate the base direction
						Vector3<float32> base_direction;

						switch (face_index)
						{
							case 0: base_direction = Vector3<float32>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._X)); break; //Front.
							case 1: base_direction = Vector3<float32>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X)); break; //Back.
							case 2: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._Y)); break; //Up.
							case 3: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y)); break; //Down.
							case 4: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, normalized_coordinate._X), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), -1.0f); break; //Right.
							case 5: base_direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._X), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, normalized_coordinate._Y), 1.0f); break; //Left.
							default: CRASH(); break;
						}

						base_direction.Normalize();

						//Sample the HDR texture.
						mip_chain[mip_level].At(face_index, X, Y) = SampleHDRTexture(hdr_texture, base_direction);
					}
				}
			}
		}
#endif
	}

	//Sewt up the base texture.
	TextureCube base_texture;
	
	{
		//Load the HDR texture.
		int32 width, height, number_of_channels;
		float32 *const RESTRICT data{ stbi_loadf(parameters._File.Data(), &width, &height, &number_of_channels, STBI_rgb_alpha)};

		//Wrap the data into a texture 2D for easier manipulation.
		Texture2D<Vector4<float32>> hdr_texture{ static_cast<uint32>(width), static_cast<uint32>(height) };

		//Copy the data into the cpu texture.
		Memory::Copy(hdr_texture.Data(), data, width * height * sizeof(Vector4<float32>));

		//Free the data, not needed anymore.
		stbi_image_free(data);

		//Create the base texture.
		base_texture.Initialize(parameters._Resolution);

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			for (uint32 Y{ 0 }; Y < parameters._Resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < parameters._Resolution; ++X)
				{
					//Define constants.
					constexpr Vector2<float32> INVERSE_ATAN{ 0.1591f, 0.3183f };

					//Calculate the direction
					Vector3<float32> direction;

					const float32 x_weight{ static_cast<float32>(X) / static_cast<float32>(parameters._Resolution) };
					const float32 y_weight{ static_cast<float32>(Y) / static_cast<float32>(parameters._Resolution) };

					switch (face_index)
					{
						case 0: direction = Vector3<float32>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight)); break; //Front.
						case 1: direction = Vector3<float32>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight)); break; //Back.
						case 2: direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, y_weight)); break; //Up.
						case 3: direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight)); break; //Down.
						case 4: direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), -1.0f); break; //Right.
						case 5: direction = Vector3<float32>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), 1.0f); break; //Left.
						default: CRASH(); break;
					}

					direction.Normalize();

					//Sample the HDR texture.
					Vector2<float32> texture_coordinate{ CatalystBaseMath::ArcTangent(direction._Z, direction._X), CatalystBaseMath::ArcSine(direction._Y) };
					texture_coordinate *= INVERSE_ATAN;
					texture_coordinate += 0.5f;

					base_texture.At(face_index, X, Y) = hdr_texture.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE);
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
	sprintf_s(directory_path, "%s\\Texture Cubes", collection_directory_path);

	//Create the compiled directory, if it doesn't exist.
	File::CreateDirectory(directory_path);
	
	//Determine the output file path.
	char output_file_path[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(output_file_path, "%s\\%s.ca", directory_path, compile_data->_Name.Data());

	//Open the output file.
	BinaryOutputFile output_file{ output_file_path };

	//Write the asset header to the file.
	AssetHeader asset_header{ AssetTypeIdentifier(), CurrentVersion(), HashString(compile_data->_Name.Data()), compile_data->_Name.Data() };
	output_file.Write(&asset_header, sizeof(AssetHeader));

	//Write the resolution to the file.
	output_file.Write(&parameters._Resolution, sizeof(uint32));

	//Write the number of mi levels to the file.
	output_file.Write(&MIP_LEVELS, sizeof(uint8));

	//Write the data to the file.
	for (uint8 mip_level{ 0 }; mip_level < MIP_LEVELS; ++mip_level)
	{
		//Calculate the mip resolution.
		const uint32 mip_resolution{ parameters._Resolution >> mip_level };

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			output_file.Write(mip_chain[mip_level].Face(face_index).Data(), mip_resolution * mip_resolution * sizeof(Vector4<float32>));
		}
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void TextureCubeAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("TextureCubeAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read the resolution.
	uint32 resolution;
	load_data->_StreamArchive->Read(&resolution, sizeof(uint32), &stream_archive_position);

	//Read the number of mip levels.
	uint8 mip_levels;
	load_data->_StreamArchive->Read(&mip_levels, sizeof(uint8), &stream_archive_position);

	//Read the data.
	DynamicArray<DynamicArray<float32>> data;

	data.Upsize<true>(mip_levels);

	for (uint8 i{ 0 }; i < mip_levels; ++i)
	{
		//Read the data.
		const uint64 number_of_elements{ (resolution >> i) * (resolution >> i) * 4 * 6 };
		data[i].Upsize<false>(number_of_elements);
		load_data->_StreamArchive->Read(data[i].Data(), number_of_elements * sizeof(float32), &stream_archive_position);
	}

	//Initialize the texture data.
	load_data->_Asset->_TextureCube.Initialize(resolution, data[0]);

	//Set the number of mip levels.
	load_data->_Asset->_MipLevels = mip_levels;

	//Create the texture cube handle.
	RenderingSystem::Instance->CreateTextureCube(resolution, data, &load_data->_Asset->_TextureCubeHandle);
}