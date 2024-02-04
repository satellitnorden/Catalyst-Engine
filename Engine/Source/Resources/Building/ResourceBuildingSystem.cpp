#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
//Header file.
#include <Resources/Building/ResourceBuildingSystem.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Animation.
#include <Animation/AnimatedVertex.h>

//Concurrency.
#include <Concurrency/Task.h>

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>
#include <File/Readers/FBXReader.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/MP4Reader.h>
#include <File/Readers/PNGReader.h>
#include <File/Readers/WAVReader.h>
#include <File/Writers/PNGWriter.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>
#include <Math/Noise/HaltonSequence.h>
#include <Math/Noise/HammersleySequence.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/TextureCube.h>
#include <Rendering/Native/Vertex.h>

//Resources
#include <Resources/Core/FontResource.h>
#include <Resources/Core/ResourcesCore.h>

//Systems.
#include <Systems/LogSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/stb_image.h>
#include <ThirdParty/stb_image_resize.h>
#include <ThirdParty/stb_truetype/stb_truetype.h>
#include <ThirdParty/vulkan/shaderc/shaderc.h>

/*
*	Builds the resource collection recursively.
*/
void BuildResourceCollectionsRecursive(	const ResourceCollectionBuildParameters &parameters,
										const char *const RESTRICT directory_path,
										BinaryFile<BinaryFileMode::OUT> *RESTRICT *RESTRICT current_file,
										uint64 *const RESTRICT file_counter,
										uint64 *const RESTRICT current_file_size) NOEXCEPT
{
	//Iterate over all files in the folder.
	for (const auto &entry : std::filesystem::directory_iterator(std::string(directory_path)))
	{
		//If this is a directory, call this function again recursively.
		if (entry.is_directory())
		{
			BuildResourceCollectionsRecursive(parameters, entry.path().generic_u8string().c_str(), current_file, file_counter, current_file_size);
		}

		else
		{
			//Open the resource file.
			BinaryFile<BinaryFileMode::IN> resource_file{ entry.path().string().c_str() };

			//Get the size of the resource file.
			const uint64 resource_file_size{ resource_file.Size() };

			//Create another resource collection file, if needed.
			if (*current_file_size + resource_file_size >= parameters._MaximumFileSize)
			{
				//Close and delete the current file.
				(*current_file)->Close();
				delete (*current_file);

				//Create a new file.
				char buffer[MAXIMUM_FILE_PATH_LENGTH];
				sprintf_s(buffer, "%s_%llu.crc", parameters._Output, (*file_counter)++);

				(*current_file) = new BinaryFile<BinaryFileMode::OUT>(buffer);

				//Reset the current file size.
				(*current_file_size) = 0;
			}

			//Read the data in the resource file.
			void *RESTRICT resource_file_data{ Memory::Allocate(resource_file_size) };
			resource_file.Read(resource_file_data, resource_file_size);

			//Write the resource file data to the resource collection file.
			(*current_file)->Write(resource_file_data, resource_file_size);

			//Free the resource file data.
			Memory::Free(resource_file_data);

			//Close the resource file.
			resource_file.Close();

			//Update the current file size.
			(*current_file_size) += resource_file_size;
		}
	}
}

/*
*	Builds resource collections.
*/
void ResourceBuildingSystem::BuildResourceCollections(const ResourceCollectionBuildParameters &parameters) NOEXCEPT
{
	//Set up variables.
	uint64 file_counter{ 0 };
	uint64 current_file_size{ 0 };

	//Create the first file.
	char buffer[MAXIMUM_FILE_PATH_LENGTH];
	sprintf_s(buffer, "%s_%llu.crc", parameters._Output, file_counter++);

	BinaryFile<BinaryFileMode::OUT> *RESTRICT current_file{ new BinaryFile<BinaryFileMode::OUT>(buffer) };

	//Build the resource collections recursively.
	BuildResourceCollectionsRecursive(parameters, parameters._Folder, &current_file, &file_counter, &current_file_size);

	//Close the file.
	current_file->Close();
}

/*
*	Builds an animated model.
*/
void ResourceBuildingSystem::BuildAnimatedModel(const AnimatedModelBuildParameters &parameters) NOEXCEPT
{
	ASSERT(false, "Not implemented!");
}

/*
*	Builds an animation.
*/
void ResourceBuildingSystem::BuildAnimation(const AnimationBuildParameters &parameters) NOEXCEPT
{
	ASSERT(false, "Not implemented!");
}

/*
*	Builds a font.
*/
void ResourceBuildingSystem::BuildFont(const FontBuildParameters &parameters) NOEXCEPT
{
	//Define constants.
	constexpr uint32 PADDING{ 4 };
	constexpr uint32 PIXEL_HEIGHT{ 64 };

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::FONT_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Open the font file.
	BinaryFile<BinaryFileMode::IN> font_file{ parameters._File };

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
		
		for (uint8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
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
		for (uint8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
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
			FontResource::CharacterDescription character_description;

			character_description._Size._X = static_cast<float32>(width) / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Size._Y = static_cast<float32>(height) / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Offset._X = static_cast<float32>(glyph_minimum._X) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);
			character_description._Offset._Y = static_cast<float32>(glyph_minimum._Y) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);
			character_description._TextureBounds._Minimum._X = static_cast<float32>(current_x) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Minimum._Y = static_cast<float32>(0) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Maximum._X = static_cast<float32>(current_x + width) / static_cast<float32>(bitmap_width);
			character_description._TextureBounds._Maximum._Y = static_cast<float32>(height) / static_cast<float32>(bitmap_height);
			character_description._Advance = static_cast<float32>(advance) * pixel_height_scale / static_cast<float32>(PIXEL_HEIGHT);

			file.Write(&character_description, sizeof(FontResource::CharacterDescription));

			//Guess this can happen. :x
			if (!glyph_sdf)
			{
				continue;
			}

			//Write into the bitmap.
			for (uint32 Y{ 0 }; Y < height; ++Y)
			{
				for (uint32 X{ 0 }; X < width; ++X)
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

	//Write the bitmap dimensions to the file.
	file.Write(&bitmap_width, sizeof(uint32));
	file.Write(&bitmap_height, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	constexpr uint8 NUMBER_OF_MIPMAP_LEVELS{ 1 };
	file.Write(&NUMBER_OF_MIPMAP_LEVELS, sizeof(uint8));

	//Write all the mip levels of the bitmap to the file.
	for (uint8 i{ 0 }; i < NUMBER_OF_MIPMAP_LEVELS; ++i)
	{
		//Write the master texture data to the file.
		file.Write(bitmap.Data(), bitmap.GetWidth() * bitmap.GetHeight());
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a level.
*/
void ResourceBuildingSystem::BuildLevel(const LevelBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._OutputFilePath };
	output_file_path_name += ".cr";

	//Open the output file to be written to.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path_name.Data() };

	//Write the resource header to the output file.
	const ResourceHeader header{ ResourceConstants::LEVEL_TYPE_IDENTIFIER, HashString(parameters._Identifier), parameters._Identifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the number of level entries to the output file.
	const uint64 number_of_level_entries{ parameters._LevelEntries.Size() };
	output_file.Write(&number_of_level_entries, sizeof(uint64));

	//Write all the level entries to the output file.
	for (const LevelEntry &level_entry : parameters._LevelEntries)
	{
		//Write the type.
		output_file.Write(&level_entry._Type, sizeof(LevelEntry::Type));

		switch (level_entry._Type)
		{
			case LevelEntry::Type::DYNAMIC_MODEL:
			{
				//Write the version.
				constexpr uint64 VERSION{ LevelEntry::DynamicModelData::CURRENT_VERSION };
				output_file.Write(&VERSION, sizeof(uint64));

				//Write the data.
				output_file.Write(&level_entry._DynamicModelData._WorldTransform, sizeof(WorldTransform));
				output_file.Write(&level_entry._DynamicModelData._ModelResourceIdentifier, sizeof(HashString));

				{
					const uint64 number_of_material_resource_identifiers{ level_entry._DynamicModelData._MaterialResourceIdentifiers.Size() };
					output_file.Write(&number_of_material_resource_identifiers, sizeof(uint64));
				}

				output_file.Write(level_entry._DynamicModelData._MaterialResourceIdentifiers.Data(), sizeof(HashString) * level_entry._DynamicModelData._MaterialResourceIdentifiers.Size());
				output_file.Write(&level_entry._DynamicModelData._ModelCollisionConfiguration, sizeof(ModelCollisionConfiguration));
				output_file.Write(&level_entry._DynamicModelData._ModelSimulationConfiguration, sizeof(ModelSimulationConfiguration));

				break;
			}

			case LevelEntry::Type::LIGHT:
			{
				//Write the version.
				constexpr uint64 VERSION{ LevelEntry::LightData::CURRENT_VERSION };
				output_file.Write(&VERSION, sizeof(uint64));

				//Write the data.
				output_file.Write(&level_entry._LightData, sizeof(LevelEntry::LightData));

				break;
			}

			case LevelEntry::Type::STATIC_MODEL:
			{
				//Write the version.
				constexpr uint64 VERSION{ LevelEntry::StaticModelData::CURRENT_VERSION };
				output_file.Write(&VERSION, sizeof(uint64));

				//Write the data.
				output_file.Write(&level_entry._StaticModelData._WorldTransform, sizeof(WorldTransform));
				output_file.Write(&level_entry._StaticModelData._ModelResourceIdentifier, sizeof(HashString));

				{
					const uint64 number_of_material_resource_identifiers{ level_entry._StaticModelData._MaterialResourceIdentifiers.Size() };
					output_file.Write(&number_of_material_resource_identifiers, sizeof(uint64));
				}

				output_file.Write(level_entry._StaticModelData._MaterialResourceIdentifiers.Data(), sizeof(HashString) * level_entry._StaticModelData._MaterialResourceIdentifiers.Size());
				output_file.Write(&level_entry._StaticModelData._ModelCollisionConfiguration, sizeof(ModelCollisionConfiguration));

				break;
			}

			case LevelEntry::Type::USER_INTERFACE:
			{
				//Write the version.
				constexpr uint64 VERSION{ LevelEntry::UserInterfaceData::CURRENT_VERSION };
				output_file.Write(&VERSION, sizeof(uint64));

				//Write the data.
				output_file.Write(&level_entry._UserInterfaceData, sizeof(LevelEntry::UserInterfaceData));

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a material.
*/
void ResourceBuildingSystem::BuildMaterial(const MaterialBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ fileName.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::MATERIAL_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the rest of the material to the file.
	file.Write(&parameters._Type, sizeof(MaterialBuildParameters) - sizeof(const char *const RESTRICT) - sizeof(const char *const RESTRICT));

	//Close the file.
	file.Close();
}

/*
*	Builds a model.
*/
void ResourceBuildingSystem::BuildModel(const ModelBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._Output };
	output_file_path_name += ".cr";

	//Open the output file to be written to.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::MODEL_TYPE_IDENTIFIER, HashString(parameters._ResourceIdentifier), parameters._ResourceIdentifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Set up all of the model files and the collision model file.
	DynamicArray<ModelFile> model_files;
	ModelFile collision_model_file;

	if (parameters._ProceduralFunction)
	{
		parameters._ProceduralFunction(&model_files, &collision_model_file, parameters._ProceduralFunctionUserData);
	}

	else
	{
		//Read all model files into memory.
		model_files.Upsize<true>(parameters._LevelOfDetails.Size());

		for (uint64 i{ 0 }, size{ model_files.Size() }; i < size; ++i)
		{
			FBXReader::Read(parameters._LevelOfDetails[i], &model_files[i]);
		}
	}

	//Transform all vertices in all meshes.
	for (ModelFile &model_file : model_files)
	{
		for (ModelFile::Mesh &mesh : model_file._Meshes)
		{
			for (Vertex &vertex : mesh._Vertices)
			{
				if (parameters._Transformation != MatrixConstants::IDENTITY || parameters._TexturCoordinateRotation != 0.0f)
				{
					vertex.Transform(parameters._Transformation, parameters._TexturCoordinateRotation);
				}

				if (parameters._TextureCoordinateMultiplier != 1.0f)
				{
					vertex._TextureCoordinate *= parameters._TextureCoordinateMultiplier;
				}
			}
		}
	}

	//Determine the model space axis aligned bounding box.
	{
		//Iterate over all vertices in all meshes and expand the bounding box.
		AxisAlignedBoundingBox3D axis_aligned_bounding_box;

		for (ModelFile &model_file : model_files)
		{
			for (ModelFile::Mesh &mesh : model_file._Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					axis_aligned_bounding_box.Expand(vertex._Position);
				}
			}
		}

		//Write the axis-aligned bounding box to the file.
		output_file.Write(&axis_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3D));
	}

	//Write the number of meshes.
	const uint64 number_of_meshes{ model_files[0]._Meshes.Size() };
	ASSERT(number_of_meshes <= RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL, "This model has more than the maximum number of meshes, either increase the maximum number of meshes or decrease the number of meshes on this model!");
	output_file.Write(&number_of_meshes, sizeof(uint64));

	//Write the number of level of details.
	const uint64 number_of_level_of_details{ model_files.Size() };
	output_file.Write(&number_of_level_of_details, sizeof(uint64));

	//Process each mesh individually.
	for (uint64 mesh_index{ 0 }; mesh_index < number_of_meshes; ++mesh_index)
	{
		for (uint64 level_of_detail_index{ 0 }; level_of_detail_index < number_of_level_of_details; ++level_of_detail_index)
		{
			//Write the number of vertices to the file.
			const uint64 number_of_vertices{ model_files[level_of_detail_index]._Meshes[mesh_index]._Vertices.Size() };
			output_file.Write(&number_of_vertices, sizeof(uint64));

			//Write the vertices to the file.
			output_file.Write(model_files[level_of_detail_index]._Meshes[mesh_index]._Vertices.Data(), sizeof(Vertex) * number_of_vertices);

			//Write the number of indices to the file.
			const uint64 number_of_indices{ model_files[level_of_detail_index]._Meshes[mesh_index]._Indices.Size() };
			output_file.Write(&number_of_indices, sizeof(uint64));

			//Write the vertices to the file.
			output_file.Write(model_files[level_of_detail_index]._Meshes[mesh_index]._Indices.Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Check if there exists a collision model. Might have already been filled in by the procedural function.
	if (parameters._CollisionModelFilePath || collision_model_file.IsValid())
	{
		//Write that there exists a collision model.
		bool collision_model_exists{ true };
		output_file.Write(&collision_model_exists, sizeof(bool));

		//Read the model file.
		if (!collision_model_file.IsValid())
		{
			FBXReader::Read(parameters._CollisionModelFilePath, &collision_model_file);
		}

		//Transform all vertices in all meshes.
		for (ModelFile::Mesh &mesh : collision_model_file._Meshes)
		{
			for (Vertex &vertex : mesh._Vertices)
			{
				if (parameters._Transformation != MatrixConstants::IDENTITY || parameters._TexturCoordinateRotation != 0.0f)
				{
					vertex.Transform(parameters._Transformation, parameters._TexturCoordinateRotation);
				}
			}
		}

		//Build the collision model.
		CollisionModelData collision_model_data;
		PhysicsSystem::Instance->BuildCollisionModel(collision_model_file, &collision_model_data);

		//Write the collision model data to the output file.
		output_file.Write(&collision_model_data._Type, sizeof(CollisionModelData::Type));

		const uint64 collision_model_data_size{ collision_model_data._Data.Size() };
		output_file.Write(&collision_model_data_size, sizeof(uint64));
		output_file.Write(collision_model_data._Data.Data(), collision_model_data_size);
	}

	else
	{
		//Write that there doesn't exist a collision model.
		bool collision_model_exists{ false };
		output_file.Write(&collision_model_exists, sizeof(bool));
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a raw data.
*/
void ResourceBuildingSystem::BuildRawData(const RawDataBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._Output };
	output_file_path_name += ".cr";

	//Open the output file to be written to.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::RAW_DATA_TYPE_IDENTIFIER, HashString(parameters._ResourceIdentifier), parameters._ResourceIdentifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the data size.
	const uint64 data_size{ sizeof(byte) * parameters._Data->Size() };
	output_file.Write(&data_size, sizeof(uint64));

	//Write the data.
	output_file.Write(parameters._Data->Data(), data_size);

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a render pipeline.
*/
void ResourceBuildingSystem::BuildRenderPipeline(const RenderPipelineBuildParameters &parameters) NOEXCEPT
{
	//What should the output file path name be?
	DynamicString output_file_path_name{ parameters._Output };
	output_file_path_name += ".cr";

	//Figure out the resource identifier.
	const char *resource_identifier{ nullptr };

	{
		const uint64 output_length{ StringUtilities::StringLength(parameters._Output) };

		for (int64 i{ static_cast<int64>(output_length) - 1 }; i >= 0; --i)
		{
			if (parameters._Output[i] == '\\')
			{
				resource_identifier = &parameters._Output[i + 1];

				break;
			}
		}
	}

	//Open the output file to be written to.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::RENDER_PIPELINE_TYPE_IDENTIFIER, HashString(resource_identifier), resource_identifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the compute shader data.
	{
		if (!parameters._ComputeShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._ComputeShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._ComputeShaderData._GLSLData.Data(), parameters._ComputeShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the vertex shader data.
	{
		if (!parameters._VertexShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._VertexShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._VertexShaderData._GLSLData.Data(), parameters._VertexShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the fragment shader data.
	{
		if (!parameters._FragmentShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._FragmentShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._FragmentShaderData._GLSLData.Data(), parameters._FragmentShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the ray generation shader data.
	{
		if (!parameters._RayGenerationShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._RayGenerationShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._RayGenerationShaderData._GLSLData.Data(), parameters._RayGenerationShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the ray miss shader data.
	{
		if (!parameters._RayMissShaderData._GLSLData.Empty())
		{
			//Write that it has GLSL shader data.
			const bool has_data{ true };
			output_file.Write(&has_data, sizeof(bool));

			//Write the data size.
			const uint64 data_size{ parameters._RayMissShaderData._GLSLData.Size() };
			output_file.Write(&data_size, sizeof(uint64));

			//Write the data.
			output_file.Write(parameters._RayMissShaderData._GLSLData.Data(), parameters._RayMissShaderData._GLSLData.Size());
		}

		else
		{
			//Write that it doesn't GLSL shader data.
			const bool has_data{ false };
			output_file.Write(&has_data, sizeof(bool));
		}
	}

	//Write the ray hit groups.
	{
		uint64 number_of_ray_hit_groups{ parameters._RayHitGroupShaderData.Size() };
		output_file.Write(&number_of_ray_hit_groups, sizeof(uint64));

		for (const RenderPipelineBuildParameters::RayHitGroupShaderData &ray_hit_group_shader_data : parameters._RayHitGroupShaderData)
		{
			//Write the identifier.
			output_file.Write(&ray_hit_group_shader_data._Identifier, sizeof(HashString));

			//Write the ray any hit shader data, if it exists.
			if (!ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Empty())
			{
				//Write that it has GLSL shader data.
				const bool has_data{ true };
				output_file.Write(&has_data, sizeof(bool));

				//Write the data size.
				const uint64 data_size{ ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Size() };
				output_file.Write(&data_size, sizeof(uint64));

				//Write the data.
				output_file.Write(ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Data(), ray_hit_group_shader_data._RayAnyHitShaderData._GLSLData.Size());
			}

			else
			{
				//Write that it doesn't GLSL shader data.
				const bool has_data{ false };
				output_file.Write(&has_data, sizeof(bool));
			}
		}
	}

	//Write the included uniform buffers.
	{
		const uint64 number_of_included_uniform_buffers{ parameters._IncludedUniformBuffers.Size() };
		output_file.Write(&number_of_included_uniform_buffers, sizeof(uint64));

		if (!parameters._IncludedUniformBuffers.Empty())
		{
			//Write the data.
			output_file.Write(parameters._IncludedUniformBuffers.Data(), sizeof(HashString) * parameters._IncludedUniformBuffers.Size());
		}
	}

	//Write the included storage buffers.
	{
		const uint64 number_of_included_storage_buffers{ parameters._IncludedStorageBuffers.Size() };
		output_file.Write(&number_of_included_storage_buffers, sizeof(uint64));

		if (!parameters._IncludedStorageBuffers.Empty())
		{
			//Write the data.
			output_file.Write(parameters._IncludedStorageBuffers.Data(), sizeof(HashString) * parameters._IncludedStorageBuffers.Size());
		}
	}

	//Write the compute render targets.
	{
		//Write the count.
		const uint64 count{ parameters._ComputeRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._ComputeRenderTargets.Data(), sizeof(HashString) * parameters._ComputeRenderTargets.Size());
		}
	}

	//Write the input render targets.
	{
		//Write the count.
		const uint64 count{ parameters._InputRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._InputRenderTargets.Data(), sizeof(Pair<HashString, SamplerProperties>) * parameters._InputRenderTargets.Size());
		}
	}

	//Write the output depth buffer.
	output_file.Write(&parameters._OutputDepthBuffer, sizeof(HashString));

	//Write the output render targets.
	{
		//Write the count.
		const uint64 count{ parameters._OutputRenderTargets.Size() };
		output_file.Write(&count, sizeof(uint64));

		//Write the data.
		if (count > 0)
		{
			output_file.Write(parameters._OutputRenderTargets.Data(), sizeof(HashString) * parameters._OutputRenderTargets.Size());
		}
	}

	//Write the render resolution.
	output_file.Write(&parameters._RenderResolution, sizeof(HashString));

	//Write the load/store operators.
	output_file.Write(&parameters._ColorLoadOperator, sizeof(AttachmentLoadOperator));
	output_file.Write(&parameters._ColorStoreOperator, sizeof(AttachmentStoreOperator));
	output_file.Write(&parameters._DepthStencilLoadOperator, sizeof(AttachmentLoadOperator));
	output_file.Write(&parameters._DepthStencilStoreOperator, sizeof(AttachmentStoreOperator));

	//Write the blend properties.
	output_file.Write(&parameters._BlendEnabled, sizeof(bool));
	output_file.Write(&parameters._BlendColorSourceFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendColorDestinationFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendColorOperator, sizeof(BlendOperator));
	output_file.Write(&parameters._BlendAlphaSourceFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendAlphaDestinationFactor, sizeof(BlendFactor));
	output_file.Write(&parameters._BlendAlphaOperator, sizeof(BlendOperator));

	//Write the cull mode.
	output_file.Write(&parameters._CullMode, sizeof(CullMode));

	//Write the depth/stencil properties.
	output_file.Write(&parameters._DepthTestEnabled, sizeof(bool));
	output_file.Write(&parameters._DepthWriteEnabled, sizeof(bool));
	output_file.Write(&parameters._DepthCompareOperator, sizeof(CompareOperator));
	output_file.Write(&parameters._StencilTestEnabled, sizeof(bool));
	output_file.Write(&parameters._StencilFailOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilPassOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilDepthFailOperator, sizeof(StencilOperator));
	output_file.Write(&parameters._StencilCompareOperator, sizeof(CompareOperator));
	output_file.Write(&parameters._StencilCompareMask, sizeof(uint32));
	output_file.Write(&parameters._StencilWriteMask, sizeof(uint32));
	output_file.Write(&parameters._StencilReferenceMask, sizeof(uint32));

	//Write the topology.
	output_file.Write(&parameters._Topology, sizeof(Topology));

	//Write the sampler properties.
	{
		const uint64 number_of_sampler_properties{ parameters._SamplerProperties.Size() };
		output_file.Write(&number_of_sampler_properties, sizeof(uint64));

		if (!parameters._SamplerProperties.Empty())
		{
			output_file.Write(parameters._SamplerProperties.Data(), sizeof(SamplerProperties) * parameters._SamplerProperties.Size());
		}
	}

	//Write the input stream subscriptions.
	{
		const uint64 number_of_input_stream_subscriptions{ parameters._InputStreamSubscriptions.Size() };
		output_file.Write(&number_of_input_stream_subscriptions, sizeof(uint64));

		if (!parameters._InputStreamSubscriptions.Empty())
		{
			output_file.Write(parameters._InputStreamSubscriptions.Data(), sizeof(HashString)* parameters._InputStreamSubscriptions.Size());
		}
	}

	//Close the output file.
	output_file.Close();
}

/*
*	Builds a shader.
*/
void ResourceBuildingSystem::BuildShader(const ShaderBuildParameters &parameters) NOEXCEPT
{
#if 0 //Use new way. (:
	//Create a compiler-ready version of the file.
	std::string shader_source;

	{
		std::ifstream file{ parameters._FilePath };

		file.seekg(0, std::ios::end);   
		shader_source.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		shader_source.assign(	std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());

		//Add the defines.
		for (const char* const RESTRICT define : parameters._Defines)
		{
			shader_source = std::string("#define ") + define + std::string("\n") + shader_source;
		}

		//Add the header data.
		{
			shader_source = "#version 460 \n#include \"C:\\Github\\Catalyst-Engine\\Engine\\Shaders\\CatalystShaderCommon.glsl\" \n" + shader_source;
		}

		//Replace "CatalystShaderMain" with "main".
		{
			size_t position{ shader_source.find("CatalystShaderMain") };

			if (position != std::string::npos)
			{
				shader_source.replace(position, strlen("CatalystShaderMain"), "main");
			}
		}
	}

	//Initialize the compiler.
	shaderc_compiler_t compiler{ shaderc_compiler_initialize() };

	//Initialize the options.
	shaderc_compile_options_t options{ shaderc_compile_options_initialize() };

	//Set the source language.
	shaderc_compile_options_set_source_language(options, shaderc_source_language::shaderc_source_language_glsl);

	//Set the optimization level.
	shaderc_compile_options_set_optimization_level(options, shaderc_optimization_level::shaderc_optimization_level_performance);

	//Determine the shader kind.
	shaderc_shader_kind shader_kind;

	switch (parameters._Stage)
	{
		case ShaderStage::COMPUTE:
		{
			shader_kind = shaderc_shader_kind::shaderc_compute_shader;

			break;
		}

		case ShaderStage::FRAGMENT:
		{
			shader_kind = shaderc_shader_kind::shaderc_fragment_shader;

			break;
		}

		case ShaderStage::GEOMETRY:
		{
			shader_kind = shaderc_shader_kind::shaderc_geometry_shader;

			break;
		}

		case ShaderStage::RAY_ANY_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_anyhit_shader;

			break;
		}

		case ShaderStage::RAY_CLOSEST_HIT:
		{
			shader_kind = shaderc_shader_kind::shaderc_closesthit_shader;

			break;
		}

		case ShaderStage::RAY_GENERATION:
		{
			shader_kind = shaderc_shader_kind::shaderc_raygen_shader;

			break;
		}

		case ShaderStage::RAY_INTERSECTION:
		{
			shader_kind = shaderc_shader_kind::shaderc_intersection_shader;

			break;
		}

		case ShaderStage::RAY_MISS:
		{
			shader_kind = shaderc_shader_kind::shaderc_miss_shader;

			break;
		}

		case ShaderStage::TESSELLATION_CONTROL:
		{
			shader_kind = shaderc_shader_kind::shaderc_tess_control_shader;

			break;
		}

		case ShaderStage::TESSELLATION_EVALUATION:
		{
			shader_kind = shaderc_shader_kind::shaderc_tess_evaluation_shader;

			break;
		}

		case ShaderStage::VERTEX:
		{
			shader_kind = shaderc_shader_kind::shaderc_vertex_shader;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Set up the include resolve function.
	auto include_resolve_function{ [](void* user_data, const char* requested_source, int type, const char* requesting_source, size_t include_depth)
	{
		//Allocate the result.
		shaderc_include_result *const RESTRICT result{static_cast<shaderc_include_result *const RESTRICT>(Memory::Allocate(sizeof(shaderc_include_result)))};

		//Absolute include paths aren't allowed.
		if (type == shaderc_include_type_standard)
		{
			result->source_name = nullptr;
			result->source_name_length = 0;
			result->content = "No absolute paths!";
			result->content_length = strlen("No absolute paths!");

			return result;
		}

		//Fill in the result.
		result->source_name = requested_source;
		result->source_name_length = strlen(requested_source);

		//Get the absolute path of the requesting source.
		std::string absolute_requesting_source{ std::filesystem::absolute(requesting_source).generic_string() };

		//Replace "/" with "\\".
		{
			for (;;)
			{
				size_t position{ absolute_requesting_source.find("/") };

				if (position == std::string::npos)
				{
					break;
				}

				absolute_requesting_source.replace(position, 1, "\\");
			}
		}

		{
			size_t position{ absolute_requesting_source.find_last_of("\\") };
			absolute_requesting_source.erase(position, absolute_requesting_source.size() - position);
		}

		//Change the working directory temporarily.
		std::filesystem::path default_working_directory{ std::filesystem::current_path() };

		//Set the working directory to the requesting source.
		std::filesystem::current_path(absolute_requesting_source);

		//Open the file.
		std::ifstream file{ requested_source };

		std::string include_source;

		file.seekg(0, std::ios::end);   
		include_source.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		include_source.assign(	std::istreambuf_iterator<char>(file),
								std::istreambuf_iterator<char>());

		//Close the file.
		file.close();

		//Reset the working directory.
		std::filesystem::current_path(default_working_directory);

		//Allocate the necessary data for the include content.
		result->content = static_cast<const char* const RESTRICT>(Memory::Allocate(include_source.size()));
		Memory::Copy(result->content, include_source.data(), include_source.size());
		result->content_length = include_source.size();

		//Return the result.
		return result;
	} };

	//Set up the include result release function.
	auto include_result_release_function{ [](void* user_data, shaderc_include_result* include_result)
	{
		//Free the content.
		Memory::Free(include_result->content);

		//Free the result.
		Memory::Free(include_result);
	} };

	//Set the include callbacks.
	shaderc_compile_options_set_include_callbacks(options, include_resolve_function, include_result_release_function, nullptr);

	//Compile!
	shaderc_compilation_result_t result{ shaderc_compile_into_spv(compiler, shader_source.c_str(), shader_source.size(), shader_kind, parameters._FilePath, "main", options) };

	//Check for errors.
	if (shaderc_result_get_num_errors(result) > 0)
	{
		ASSERT(false, shaderc_result_get_error_message(result));
	}

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::SHADER_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the stage.
	file.Write(&parameters._Stage, sizeof(ShaderStage));

	//Write the size of the compiled file.
	const uint64 compiled_file_size{ shaderc_result_get_length(result) };
	file.Write(&compiled_file_size, sizeof(uint64));

	//Read the data.
	DynamicArray<byte> data;
	data.Upsize<false>(compiled_file_size);

	Memory::Copy(data.Data(), shaderc_result_get_bytes(result), compiled_file_size);

	//Write the data.
	file.Write(data.Data(), compiled_file_size);

	//Close the file.
	file.Close();

	//Release the result.
	shaderc_result_release(result);

	//Release the options.
	shaderc_compile_options_release(options);

	//Release the compiler.
	shaderc_compiler_release(compiler);
#else
	//Determine the temporary shader file path.
	DynamicString temporary_shader_file_path{ "C:\\Github\\Catalyst-Engine\\Engine\\Shaders\\" };
	temporary_shader_file_path += parameters._ID;
	temporary_shader_file_path += ".glsl";

	//Determine the compiled file path.
	DynamicString compiled_file_path{ parameters._ID };
	compiled_file_path += ".compiled";

	//Determine the temporary batch file path.
	DynamicString temporary_batch_file_path{ parameters._ID };
	temporary_batch_file_path += ".bat";

	//First, compile the shader.
	{
		//Create a compiler-ready version of the file.
		{
			std::ifstream file{ parameters._FilePath };
			std::string file_string;

			file.seekg(0, std::ios::end);   
			file_string.reserve(file.tellg());
			file.seekg(0, std::ios::beg);

			file_string.assign(	(std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());

			//Add the defines.
			for (const char* const RESTRICT define : parameters._Defines)
			{
				file_string = std::string("#define ") + define + std::string("\n") + file_string;
			}

			//Add the header data.
			{
				file_string = "#version 460 \n#extension GL_GOOGLE_include_directive : enable \n#include \"CatalystShaderCommon.glsl\" \n" + file_string;
			}

			//Replace "CatalystShaderMain" with "main".
			{
				size_t position{ file_string.find("CatalystShaderMain") };

				if (position != std::string::npos)
				{
					file_string.replace(position, strlen("CatalystShaderMain"), "main");
				}
			}

			//Write the compiler-ready version to the temporary shader file.
			std::ofstream shader_file{ temporary_shader_file_path.Data() };

			shader_file << file_string;

			shader_file.close();
		}

		//Compile the file.
		{
			//Create a temporary batch file that stores all commands.
			std::ofstream batch_file;

			batch_file.open(temporary_batch_file_path.Data(), std::ios::out);

			batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
			batch_file << " -V ";
			batch_file << temporary_shader_file_path.Data();
			batch_file << " -o ";
			batch_file << compiled_file_path.Data();

			switch (parameters._Stage)
			{
				case ShaderStage::COMPUTE:
				{
					batch_file << " -S comp";

					break;
				}

				case ShaderStage::FRAGMENT:
				{
					batch_file << " -S frag";

					break;
				}

				case ShaderStage::GEOMETRY:
				{
					batch_file << " -S geom";

					break;
				}

				case ShaderStage::RAY_ANY_HIT:
				{
					batch_file << " -S rahit";

					break;
				}

				case ShaderStage::RAY_CLOSEST_HIT:
				{
					batch_file << " -S rchit";

					break;
				}

				case ShaderStage::RAY_GENERATION:
				{
					batch_file << " -S rgen";

					break;
				}

				case ShaderStage::RAY_INTERSECTION:
				{
					batch_file << " -S rint";

					break;
				}

				case ShaderStage::RAY_MISS:
				{
					batch_file << " -S rmiss";

					break;
				}

				case ShaderStage::TESSELLATION_CONTROL:
				{
					batch_file << " -S tesc";

					break;
				}

				case ShaderStage::TESSELLATION_EVALUATION:
				{
					batch_file << " -S tese";

					break;
				}

				case ShaderStage::VERTEX:
				{
					batch_file << " -S vert";

					break;
				}
			}

			batch_file.close();

			system(temporary_batch_file_path.Data());

			//Delete the temporary batch file.
			File::Delete(temporary_batch_file_path.Data());
		}
	}

	//If the file exists, recreate the temporary batch file with a pause statement in the end to display the error.
	if (!File::Exists(compiled_file_path.Data()))
	{
		std::ofstream batch_file;

		batch_file.open(temporary_batch_file_path.Data(), std::ios::out);

		batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
		batch_file << " -V ";
		batch_file << temporary_shader_file_path.Data();
		batch_file << " -o ";
		batch_file << compiled_file_path.Data();

		switch (parameters._Stage)
		{
			case ShaderStage::COMPUTE:
			{
				batch_file << " -S comp";

				break;
			}

			case ShaderStage::FRAGMENT:
			{
				batch_file << " -S frag";

				break;
			}

			case ShaderStage::GEOMETRY:
			{
				batch_file << " -S geom";

				break;
			}

			case ShaderStage::RAY_ANY_HIT:
			{
				batch_file << " -S rahit";

				break;
			}

			case ShaderStage::RAY_CLOSEST_HIT:
			{
				batch_file << " -S rchit";

				break;
			}

			case ShaderStage::RAY_GENERATION:
			{
				batch_file << " -S rgen";

				break;
			}

			case ShaderStage::RAY_INTERSECTION:
			{
				batch_file << " -S rint";

				break;
			}

			case ShaderStage::RAY_MISS:
			{
				batch_file << " -S rmiss";

				break;
			}

			case ShaderStage::TESSELLATION_CONTROL:
			{
				batch_file << " -S tesc";

				break;
			}

			case ShaderStage::TESSELLATION_EVALUATION:
			{
				batch_file << " -S tese";

				break;
			}

			case ShaderStage::VERTEX:
			{
				batch_file << " -S vert";

				break;
			}
		}

		batch_file << std::endl;
		batch_file << "pause";

		batch_file.close();

		system(temporary_batch_file_path.Data());
	}

	//Delete the temporary shader file.
	File::Delete(temporary_shader_file_path.Data());

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::SHADER_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the stage.
	file.Write(&parameters._Stage, sizeof(ShaderStage));

	//Open the compiled file.
	BinaryFile<BinaryFileMode::IN> compiled_file{ compiled_file_path.Data() };

	//Write the size of the compiled file.
	const uint64 compiled_file_size{ compiled_file.Size() };
	file.Write(&compiled_file_size, sizeof(uint64));

	//Read the data.
	DynamicArray<byte> data;
	data.Upsize<false>(compiled_file_size);

	compiled_file.Read(data.Data(), compiled_file_size);

	//Write the data.
	file.Write(data.Data(), compiled_file_size);

	//Close the compiled file.
	compiled_file.Close();

	//Close the file.
	file.Close();

	//Delete the compiled file.
	File::Delete(compiled_file_path.Data());
#endif
}

/*
*	Builds a sound.
*/
void ResourceBuildingSystem::BuildSound(const SoundBuildParameters &parameters) NOEXCEPT
{
	//Read the file.
	SoundResource resource;

	if (WAVReader::Read(parameters._File, &resource))
	{
		//Resample the sound resource to the desired sample rate.
		if (resource._SampleRate != parameters._DesiredSampleRate)
		{
			const float32 playback_rate{ resource._SampleRate / parameters._DesiredSampleRate };

			DynamicArray<DynamicArray<int16>> temporary_samples{ resource._Samples };

			for (uint64 i{ 0 }, size{ temporary_samples.Size() }; i < size; ++i)
			{
				resource._Samples[i].Clear();

				DynamicArray<int16>& channel{ temporary_samples[i] };

				uint64 current_sample{ 0 };
				float32 current_sample_fraction{ 0.0f };

				while (current_sample < channel.Size())
				{
					const int16 first_sample{ channel[current_sample] };
					const int16 second_sample{ channel[current_sample < channel.Size() - 1 ? current_sample + 1 : current_sample] };

					const int16 interpolated_sample{ CatalystBaseMath::Round<int16>(CatalystBaseMath::LinearlyInterpolate(static_cast<float32>(first_sample), static_cast<float32>(second_sample), current_sample_fraction)) };
				
					resource._Samples[i].Emplace(interpolated_sample);

					current_sample_fraction += playback_rate;

					while (current_sample_fraction >= 1.0f)
					{
						++current_sample;
						current_sample_fraction -= 1.0f;
					}
				}
			}
		}

		//What should the resource be called?
		DynamicString file_name{ parameters._Output };
		file_name += ".cr";

		//Open the file to be written to.
		BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

		//Write the resource header to the file.
		const ResourceHeader header{ ResourceConstants::SOUND_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
		file.Write(&header, sizeof(ResourceHeader));

		//Write the sample rate to the file.
		file.Write(&parameters._DesiredSampleRate, sizeof(float32));

		//Write the number of channels to the file.
		file.Write(&resource._NumberOfChannels, sizeof(uint8));

		//Write the number of samples.
		const uint64 number_of_samples{ resource._Samples[0].Size() }; //Assume all channels have the same size.
		file.Write(&number_of_samples, sizeof(uint64));

		//Write the samples.
		for (const DynamicArray<int16>& channel : resource._Samples)
		{
			file.Write(channel.Data(), sizeof(int16) * number_of_samples);
		}

		//Close the file.
		file.Close();
	}

	else
	{
		ASSERT(false, "Couldn't build sound with output; " << parameters._Output << ", ID; " << parameters._ID << ", file path; " << parameters._File);
	}
}

/*
*	Builds a texture cube
*/
void ResourceBuildingSystem::BuildTextureCube(const TextureCubeBuildParameters &parameters) NOEXCEPT
{
	//Define constants.
	constexpr uint8 MIPMAP_LEVELS{ 3 };
	constexpr uint32 BASE_RESOLUTION{ 512 };
	constexpr StaticArray<Vector2<float32>, 4> SUPER_SAMPLE_OFFSETS
	{
		Vector2<float32>(-0.25f, -0.25f),
		Vector2<float32>(-0.25f, 0.25f),
		Vector2<float32>(0.25f, -0.25f),
		Vector2<float32>(0.25f, 0.25f)
	};

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Cache the resolution.
	uint32 resolution;

	if (parameters._ProceduralFunction)
	{
		resolution = parameters._DefaultResolution;
	}

	else
	{
		resolution = BASE_RESOLUTION;
	}

	//Set up the base texture.
	TextureCube base_texture;

	if (parameters._ProceduralFunction)
	{
		base_texture.Initialize(resolution);

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			for (uint32 Y{ 0 }; Y < resolution; ++Y)
			{
				LOG_INFORMATION("Processing face #%u and Y: %u", static_cast<uint32>(face_index + 1), Y);

				for (uint32 X{ 0 }; X < resolution; ++X)
				{
					//Calculate the direction
					const Vector2<float32> normalized_coordinate{	(static_cast<float32>(X) + 0.5f) / static_cast<float32>(resolution),
																	(static_cast<float32>(Y) + 0.5f) / static_cast<float32>(resolution) };

					//Retrieve the color.
					Vector4<float32> color;

					if (parameters._ProceduralFunctionSuperSample)
					{
						Vector4<float32> average_color{ 0.0f, 0.0f, 0.0f, 0.0f };

						for (uint8 i{ 0 }; i < 4; ++i)
						{
							const Vector3<float32> direction{ base_texture.GetDirection(face_index, normalized_coordinate + (SUPER_SAMPLE_OFFSETS[i] * (1.0f / static_cast<float32>(resolution)))) };

							average_color += parameters._ProceduralFunction(direction, parameters._ProceduralFunctionUserData);
						}

						color = average_color * 0.25f;
					}
					
					else
					{
						const Vector3<float32> direction{ base_texture.GetDirection(face_index, normalized_coordinate) };

						color = parameters._ProceduralFunction(direction, parameters._ProceduralFunctionUserData);
					}

					//Write the color.
					base_texture.At(face_index, X, Y) = color;
				}
			}
		}
	}
	
	else
	{
		//Load the HDR texture.
		int32 width, height, number_of_channels;
		float32 *const RESTRICT data{ stbi_loadf(parameters._File, &width, &height, &number_of_channels, STBI_rgb_alpha) };

		//Wrap the data into a texture 2D for easier manipulation.
		Texture2D<Vector4<float32>> hdr_texture{ static_cast<uint32>(width), static_cast<uint32>(height) };

		//Copy the data into the cpu texture.
		Memory::Copy(hdr_texture.Data(), data, width * height * 4 * sizeof(float32));

		//Free the data, not needed anymore.
		stbi_image_free(data);

		//Create the base texture.
		base_texture.Initialize(resolution);

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			for (uint32 Y{ 0 }; Y < resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < resolution; ++X)
				{
					//Define constants.
					constexpr Vector2<float32> INVERSE_ATAN{ 0.1591f, 0.3183f };

					//Calculate the direction
					Vector3<float32> direction;

					const float32 x_weight{ static_cast<float32>(X) / static_cast<float32>(resolution) };
					const float32 y_weight{ static_cast<float32>(Y) / static_cast<float32>(resolution) };

					switch (face_index)
					{
						default: CRASH(); break;
						case 0: direction = Vector3<float>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight)); break; //Front.
						case 1: direction = Vector3<float>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight)); break; //Back.
						case 2: direction = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, y_weight)); break; //Up.
						case 3: direction = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight)); break; //Down.
						case 4: direction = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), -1.0f); break; //Right.
						case 5: direction = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), 1.0f); break; //Left.
					}

					direction.Normalize();

					//Sample the HDR texture.
					Vector2<float> texture_coordinate{ CatalystBaseMath::ArcTangent(direction._Z, direction._X), CatalystBaseMath::ArcSine(direction._Y) };
					texture_coordinate *= INVERSE_ATAN;
					texture_coordinate += 0.5f;

					base_texture.At(face_index, X, Y) = hdr_texture.Sample(texture_coordinate, AddressMode::CLAMP_TO_EDGE);
				}
			}
		}
	}

	//Create the mip chain.
	StaticArray<TextureCube, MIPMAP_LEVELS - 1> mip_chain;

	for (uint8 mipmap_level{ 0 }; mipmap_level < MIPMAP_LEVELS - 1; ++mipmap_level)
	{
		//Calculate the mip resolution.
		const uint32 mip_resolution{ resolution >> (mipmap_level + 1) };

		//Initialize the mip texture.
		mip_chain[mipmap_level].Initialize(mip_resolution);

		//Calculate the sample delta.
		const Vector2<float32> sample_delta{ 1.0f / static_cast<float32>(resolution), 1.0f / static_cast<float32>(resolution) };

		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			const Texture2D<Vector4<float32>> &previous_texture{ mipmap_level == 0 ? base_texture.Face(face_index) : mip_chain[mipmap_level - 1].Face(face_index) };

			for (uint32 Y{ 0 }; Y < mip_resolution; ++Y)
			{
				for (uint32 X{ 0 }; X < mip_resolution; ++X)
				{
					//Calculate the normalized coordinate.
					const Vector2<float32> normalized_coordinate{ (static_cast<float32>(X) + 0.5f) / static_cast<float32>(mip_resolution), (static_cast<float32>(Y) + 0.5f) / static_cast<float32>(mip_resolution) };

					Vector4<float32> total{ 0.0f, 0.0f, 0.0f, 0.0f };

					for (int32 sub_y{ -1 }; sub_y <= 1; ++sub_y)
					{
						for (int32 sub_x{ -1 }; sub_x <= 1; ++sub_x)
						{
							total += previous_texture.Sample(normalized_coordinate + Vector2<float32>(-1.0f, -1.0f) * sample_delta, AddressMode::CLAMP_TO_EDGE);
						}
					}

					mip_chain[mipmap_level].At(face_index, X, Y) = total / 9.0f;
				}
			}
		}
	}

	//Write the resolution to the file.
	file.Write(&resolution, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the data to the file.
	for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
	{
		file.Write(base_texture.Face(face_index).Data(), resolution * resolution * sizeof(Vector4<float32>));
	}

	for (uint8 mipmap_level{ 0 }; mipmap_level < MIPMAP_LEVELS - 1; ++mipmap_level)
	{
		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			file.Write(mip_chain[mipmap_level].Face(face_index).Data(), (resolution >> (mipmap_level + 1)) * (resolution >> (mipmap_level + 1)) * sizeof(Vector4<float32>));
		}
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a texture 2D.
*/
void ResourceBuildingSystem::BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT
{
	//Load the input textures.
	StaticArray<Texture2D<Vector4<float32>>, 4> input_textures;

	if (parameters._File1)
	{
		ASSERT(File::Exists(parameters._File1), "File " << parameters._File1 << " does not exist!");

		switch (File::GetExtension(parameters._File1))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File1, &input_textures[0]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File1, &input_textures[0]);

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
		ASSERT(File::Exists(parameters._File2), "File " << parameters._File2 << " does not exist!");

		switch (File::GetExtension(parameters._File2))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File2, &input_textures[1]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File2, &input_textures[1]);

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
		ASSERT(File::Exists(parameters._File3), "File " << parameters._File3 << " does not exist!");

		switch (File::GetExtension(parameters._File3))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File3, &input_textures[2]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File3, &input_textures[2]);

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
		ASSERT(File::Exists(parameters._File4), "File " << parameters._File4 << " does not exist!");

		switch (File::GetExtension(parameters._File4))
		{
			case File::Extension::JPG:
			{
				JPGReader::Read(parameters._File4, &input_textures[3]);

				break;
			}

			case File::Extension::PNG:
			{
				PNGReader::Read(parameters._File4, &input_textures[3]);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	//Determine the width and height. Prefer if any of the input textures can report the dimensions, otherwise use the defaults.
	uint32 width{ 0 };
	uint32 height{ 0 };

	for (uint8 i{ 0 }; i < 4; ++i)
	{
		if (input_textures[i].GetWidth() > 0 && input_textures[i].GetHeight() > 0)
		{
			width = input_textures[i].GetWidth();
			height = input_textures[i].GetHeight();
		}
	}
	
	if (width == 0 && height == 0)
	{
		width = parameters._DefaultWidth;
		height = parameters._DefaultHeight;
	}

	if (width == 0 && height == 0)
	{
		ASSERT(false, "Invalid resolution!");
	}

	//Create the composite texture. Assume that all input textures are the same size.
	Texture2D<Vector4<float32>> composite_texture{ width, height };

	for (uint32 Y{ 0 }; Y < composite_texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < composite_texture.GetWidth(); ++X)
		{
			//Calculate the texel.
			Vector4<float32> &texel{ composite_texture.At(X, Y) };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (UNDERLYING(parameters._ChannelMappings[i]._File) <= UNDERLYING(Texture2DBuildParameters::File::FILE_4)
					&& input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].GetWidth() > 0
					&& input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].GetHeight() > 0)
				{
					texel[i] = input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].At(X, Y)[UNDERLYING(parameters._ChannelMappings[i]._Channel)] * parameters._Tint[i];
				}

				else
				{
					texel[i] = parameters._Default[UNDERLYING(parameters._ChannelMappings[i]._Channel)] * parameters._Tint[i];
				}
			}
		}
	}

	//Apply the transform function.
	if (parameters._TransformFunction)
	{
		Texture2D<Vector4<float32>> temporary_composite_texture{ composite_texture };

		parameters._TransformFunction(temporary_composite_texture, &composite_texture);
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

	for (uint64 i{ 0 }, size{ output_textures.Size() }; i < size; ++i)
	{
		output_textures[i].Initialize(mip_chain[i].GetWidth(), mip_chain[i].GetHeight());

		for (uint32 Y{ 0 }; Y < output_textures[i].GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < output_textures[i].GetWidth(); ++X)
			{
				for (uint8 j{ 0 }; j < 4; ++j)
				{
					output_textures[i].At(X, Y)[j] = static_cast<uint8>(mip_chain[i].At(X, Y)[j] * static_cast<float32>(UINT8_MAXIMUM));
				}
			}
		}
	}

	//What should the file be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_2D_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the number of mipmap levels to the file.
	const uint8 mipmap_levels{ static_cast<uint8>(output_textures.Size() - parameters._BaseMipmapLevel)};
	file.Write(&mipmap_levels, sizeof(uint8));

	//Write the width and height of the texture to the file.
	const uint32 output_width{ output_textures[parameters._BaseMipmapLevel].GetWidth() };
	file.Write(&output_width, sizeof(uint32));
	const uint32 output_height{ output_textures[parameters._BaseMipmapLevel].GetHeight() };
	file.Write(&output_height, sizeof(uint32));

	//Write the texture data to the file.
	for (uint64 i{ parameters._BaseMipmapLevel }, size{ output_textures.Size() }; i < size; ++i)
	{
		file.Write(output_textures[i].Data(), sizeof(Vector4<uint8>) * output_textures[i].GetWidth() * output_textures[i].GetHeight());
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a texture 3D.
*/
void ResourceBuildingSystem::BuildTexture3D(const Texture3DBuildParameters& parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the number of mipmap levels to the file.
	constexpr uint8 MIPMAP_LEVELS{ 1 };
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the width, height and depth.
	const uint32 width{ parameters._Texture->GetWidth() };
	const uint32 height{ parameters._Texture->GetHeight() };
	const uint32 depth{ parameters._Texture->GetDepth() };

	file.Write(&width, sizeof(uint32));
	file.Write(&height, sizeof(uint32));
	file.Write(&depth, sizeof(uint32));

	//Write the data.
	file.Write(parameters._Texture->Data(), width * height * depth * sizeof(Vector4<byte>));

	//Close the output file.
	file.Close();
}

/*
*	Builds a video.
*/
void ResourceBuildingSystem::BuildVideo(const VideoBuildParameters &parameters) NOEXCEPT
{
	//Read the video resource.
	VideoResource video_resource;
	MP4Reader::Read(parameters._FilePath, &video_resource);

	//What should the output file path be?
	DynamicString output_file_path{ parameters._Output };
	output_file_path += ".cr";

	//Open the output file to be written to.
	BinaryFile<BinaryFileMode::OUT> output_file{ output_file_path.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::VIDEO_TYPE_IDENTIFIER, HashString(parameters._ResourceIdentifier), parameters._ResourceIdentifier };
	output_file.Write(&header, sizeof(ResourceHeader));

	//Write the width.
	output_file.Write(&video_resource._Width, sizeof(uint32));

	//Write the height.
	output_file.Write(&video_resource._Height, sizeof(uint32));

	//Write the frames per second.
	output_file.Write(&video_resource._FramesPerSecond, sizeof(float32));

	//Write the frames per second reciprocal.
	output_file.Write(&video_resource._FramesPerSecondReciprocal, sizeof(float32));

	//Write the number of frames.
	const uint64 number_of_frames{ video_resource._Frames.Size() };
	output_file.Write(&number_of_frames, sizeof(uint64));

	//Write the frames.
	for (const VideoResource::Frame &frame : video_resource._Frames)
	{
		output_file.Write(frame._DataX.Data(), sizeof(uint8) * video_resource._Width * video_resource._Height);
		output_file.Write(frame._DataY.Data(), sizeof(uint8) * video_resource._Width * video_resource._Height);
		output_file.Write(frame._DataZ.Data(), sizeof(uint8) * video_resource._Width * video_resource._Height);
	}

	//Close the output file.
	output_file.Close();
}
#endif