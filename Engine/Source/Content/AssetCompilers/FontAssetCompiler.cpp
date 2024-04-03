//Header file.
#include <Content/AssetCompilers/FontAssetCompiler.h>

//File.
#include <File/Core/File.h>
#include <File/Core/BinaryOutputFile.h>
#include <File/Core/BinaryInputFile.h>
#include <File/Utilities/TextParsingUtilities.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/stb_truetype/stb_truetype.h>

//STL.
#include <fstream>

//Singleton definition.
DEFINE_SINGLETON(FontAssetCompiler);

/*
*	Font parameters class definition.
*/
class FontParameters final
{

public:

	//The file.
	StaticString<MAXIMUM_FILE_PATH_LENGTH> _File;

};

/*
*	Default constructor.
*/
FontAssetCompiler::FontAssetCompiler() NOEXCEPT
{
	//Set the flags.
	_Flags = Flags::NONE;
}

/*
*	Returns the asset type identifier.
*/
NO_DISCARD HashString FontAssetCompiler::AssetTypeIdentifier() const NOEXCEPT
{
	return FontAsset::TYPE_IDENTIFIER;
}

/*
*	Returns the current version.
*/
NO_DISCARD uint64 FontAssetCompiler::CurrentVersion() const NOEXCEPT
{
	return 1;
}

/*
*	Compiles a single asset with the given compile context.
*/
void FontAssetCompiler::Compile(const CompileContext &compile_context) NOEXCEPT
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
		FontAssetCompiler::Instance->CompileInternal(static_cast<CompileData *const RESTRICT>(arguments));
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
NO_DISCARD Asset *const RESTRICT FontAssetCompiler::Load(const LoadContext &load_context) NOEXCEPT
{
	//Allocate the asset.
	FontAsset *const RESTRICT new_asset{ new (_AssetAllocator.Allocate()) FontAsset() };

	//Set up the load data.
	LoadData *const RESTRICT load_data{ new (_LoadDataAllocator.Allocate()) LoadData() };

	load_data->_StreamArchivePosition = load_context._StreamArchivePosition;
	load_data->_StreamArchive = load_context._StreamArchive;
	load_data->_Asset = new_asset;

	//Set up the task.
	Task *const RESTRICT task{ static_cast<Task *const RESTRICT>(load_context._TaskAllocator->Allocate()) };

	task->_Function = [](void *const RESTRICT arguments)
	{
		FontAssetCompiler::Instance->LoadInternal(static_cast<LoadData *const RESTRICT>(arguments));
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

/*
*	Compiles internally.
*/
void FontAssetCompiler::CompileInternal(CompileData *const RESTRICT compile_data) NOEXCEPT
{
	PROFILING_SCOPE("FontAssetCompiler::CompileInternal");

	//Define constants.
	constexpr uint32 PADDING{ 4 };
	constexpr uint32 PIXEL_HEIGHT{ 64 };

	//Set up the parameters.
	FontParameters parameters;

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

			//Couldn't figure out what this line is?
			ASSERT(false, "Unknown line %s", current_line.c_str());
		}
	}

	//Close the input file.
	input_file.close();

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
	sprintf_s(directory_path, "%s\\Fonts", collection_directory_path);

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

	//Open the font file.
	BinaryInputFile font_file{ parameters._File.Data()};

	//Create the buffer.
	byte *const RESTRICT buffer{ static_cast<byte *const RESTRICT>(Memory::Allocate(font_file.Size())) };
	font_file.Read(buffer, font_file.Size());

	//Close the font file.
	font_file.Close();

	//Initialize the font.
	stbtt_fontinfo font_info;
	stbtt_InitFont(&font_info, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));

	//Calculate the pixel height scale.
	const float32 pixel_height_scale{ stbtt_ScaleForPixelHeight(&font_info, PIXEL_HEIGHT) };

	//Calculate the bitmap width/height.
	uint32 bitmap_width;
	uint32 bitmap_height;

	{
		//Do a run to calculate the needed width of the bitmap.
		uint32 needed_width{ 0 };

		for (uint8 i{ 0 }; i < FontAsset::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
		{
			//Retrieve the glyph SDF.
			int32 width;
			int32 height;
			int32 x_offset;
			int32 y_offset;

			uint8 *const RESTRICT glyph_sdf
			{
				stbtt_GetCodepointSDF
				(
					&font_info,
					pixel_height_scale,
					i,
					PADDING,
					127,
					127.0f / static_cast<float32>(PADDING / 2),
					&width,
					&height,
					&x_offset,
					&y_offset
				)
			};

			//Guess this can happen. :x
			if (!glyph_sdf)
			{
				continue;
			}

			//Update the needed width.
			needed_width += width;

			//Free the glyph data.
			stbtt_FreeSDF(glyph_sdf, nullptr);
		}

		//Set the bitmap width/height.
		bitmap_width = needed_width;
		bitmap_height = PIXEL_HEIGHT;
	}

	//Set up the bitmap.
	Texture2D<uint8> bitmap{ bitmap_width, bitmap_height };

	//Clear it. Feels safer. :x
	Memory::Set(bitmap.Data(), 0, bitmap.GetWidth() * bitmap.GetHeight());

	{
		//The current X.
		uint32 current_x{ 0 };

		//Writes all the glyphs for the bitmap.
		for (uint8 i{ 0 }; i < FontAsset::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
		{
			//Retrieve the glyph SDF.
			int32 width{ 0 };
			int32 height{ 0 };
			int32 x_offset{ 0 };
			int32 y_offset{ 0 };

			uint8 *const RESTRICT glyph_sdf
			{
				stbtt_GetCodepointSDF
				(
					&font_info,
					pixel_height_scale,
					i,
					PADDING,
					127,
					127.0f / static_cast<float32>(PADDING),
					&width,
					&height,
					&x_offset,
					&y_offset
				)
			};

			//Retrieve the bounding box for the glyph.
			Vector2<int32> glyph_minimum;
			Vector2<int32> glyph_maximum;
			stbtt_GetCodepointBox(&font_info, i, &glyph_minimum._X, &glyph_minimum._Y, &glyph_maximum._X, &glyph_maximum._Y);

			//Retrieve the advance.
			int32 advance;
			stbtt_GetCodepointHMetrics(&font_info, i, &advance, nullptr);

			//Write the character description to the file.
			FontAsset::CharacterDescription character_description;

			character_description._Size._X = static_cast<float32>(width) / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Size._Y = static_cast<float32>(height) / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Offset._X = static_cast<float32>(glyph_minimum._X) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Offset._Y = static_cast<float32>(glyph_minimum._Y) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);
			character_description._TextureBounds._Minimum._X = static_cast<float32>(current_x) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Minimum._Y = static_cast<float32>(0) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Maximum._X = static_cast<float32>(current_x + width) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Maximum._Y = static_cast<float32>(height) / static_cast<float32>(bitmap_height);
			character_description._Advance = static_cast<float32>(advance) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);

			output_file.Write(&character_description, sizeof(FontAsset::CharacterDescription));

			//Guess this can happen. :x
			if (!glyph_sdf)
			{
				continue;
			}

			//Write into the bitmap.
			for (uint32 Y{ 0 }; Y < static_cast<uint32>(height); ++Y)
			{
				for (uint32 X{ 0 }; X < static_cast<uint32>(width); ++X)
				{
					bitmap.At(current_x + X, Y) = glyph_sdf[X + ((height - 1 - Y) * width)];
				}
			}

			//Update the current X.
			current_x += width;

			//Free the glyph data.
			stbtt_FreeSDF(glyph_sdf, nullptr);
		}
	}

#if 0
	//Write it out, for debug purposes.
	{
		Texture2D<Vector4<float32>> debug_texture{ bitmap.GetWidth(), bitmap.GetHeight() };

		for (uint32 Y{ 0 }; Y < bitmap.GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < bitmap.GetWidth(); ++X)
			{
				const float32 value{ static_cast<float32>(bitmap.At(X, Y)) / static_cast<float32>(UINT8_MAXIMUM) };
				debug_texture.At(X, Y) = Vector4<float32>(value, value, value, 1.0f);
			}
		}

		PNGWriter::Write(debug_texture, "Font_Debug.png");
	}
#endif

	//Write the bitmap dimensions to the file.
	output_file.Write(&bitmap_width, sizeof(uint32));
	output_file.Write(&bitmap_height, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	constexpr uint8 NUMBER_OF_MIPMAP_LEVELS{ 1 };
	output_file.Write(&NUMBER_OF_MIPMAP_LEVELS, sizeof(uint8));

	//Write all the mip levels of the bitmap to the file.
	for (uint8 i{ 0 }; i < NUMBER_OF_MIPMAP_LEVELS; ++i)
	{
		//Write the master texture data to the file.
		output_file.Write(bitmap.Data(), bitmap.GetWidth() * bitmap.GetHeight());
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Loads internally.
*/
void FontAssetCompiler::LoadInternal(LoadData *const RESTRICT load_data) NOEXCEPT
{
	PROFILING_SCOPE("FontAssetCompiler::LoadInternal");

	//Read the data.
	uint64 stream_archive_position{ load_data->_StreamArchivePosition };

	//Read all characters.
	for (int8 i{ 0 }; i < FontAsset::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		//Read the character description.
		load_data->_StreamArchive->Read(&load_data->_Asset->_CharacterDescriptions[i], sizeof(FontAsset::CharacterDescription), &stream_archive_position);
	}

	//Read the texture width.
	uint32 texture_width;
	load_data->_StreamArchive->Read(&texture_width, sizeof(uint32), &stream_archive_position);

	//Read the master texture height.
	uint32 texture_height;
	load_data->_StreamArchive->Read(&texture_height, sizeof(uint32), &stream_archive_position);

	//Read the number of mipmap levels.
	uint8 number_of_mipmap_levels;
	load_data->_StreamArchive->Read(&number_of_mipmap_levels, sizeof(uint8), &stream_archive_position);

	//Read the texture data.
	DynamicArray<DynamicArray<byte>> texture_data;
	texture_data.Upsize<true>(number_of_mipmap_levels);

	for (uint8 i{ 0 }; i < number_of_mipmap_levels; ++i)
	{
		const uint32 mip_width{ texture_width >> i };
		const uint32 mip_height{ texture_height >> i };

		texture_data[i].Upsize<false>(mip_width * mip_height);

		load_data->_StreamArchive->Read(texture_data[i].Data(), mip_width * mip_height, &stream_archive_position);
	}

	//Create the texture.
	Texture2DHandle texture;
	RenderingSystem::Instance->CreateTexture2D(TextureData(TextureDataContainer(texture_data, texture_width, texture_height, 1), TextureFormat::R_UINT8, TextureUsage::NONE, false), &texture);

	load_data->_Asset->_MasterTextureIndex = RenderingSystem::Instance->AddTextureToGlobalRenderData(texture);

	//Calculate the default height.
	load_data->_Asset->_DefaultHeight = FLOAT32_MAXIMUM;

	for (uint8 i{ 0 }; i < FontAsset::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		if (load_data->_Asset->_CharacterDescriptions[i]._Size._Y != 0.0f
			&& ((i >= 'A' && i <= 'Z')
				|| (i >= 'a' && i <= 'z')))
		{
			load_data->_Asset->_DefaultHeight = CatalystBaseMath::Minimum<float32>(load_data->_Asset->_DefaultHeight, load_data->_Asset->_CharacterDescriptions[i]._Size._Y);
		}
	}

	if (load_data->_Asset->_DefaultHeight == FLOAT32_MAXIMUM)
	{
		ASSERT(false, "This shouldn't happen...");

		load_data->_Asset->_DefaultHeight = 0.0f;

		float32 number_of_valid_characters{ 0.0f };

		for (uint8 i{ 0 }; i < FontAsset::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
		{
			if (load_data->_Asset->_CharacterDescriptions[i]._Size._Y != 0.0f)
			{
				load_data->_Asset->_DefaultHeight += load_data->_Asset->_CharacterDescriptions[i]._Size._Y;

				++number_of_valid_characters;
			}
		}

		if (number_of_valid_characters > 0.0f)
		{
			load_data->_Asset->_DefaultHeight /= number_of_valid_characters;
		}

		else
		{
			ASSERT(false, "This shouldn't happen...");

			load_data->_Asset->_DefaultHeight = 0.1f;
		}
	}
}