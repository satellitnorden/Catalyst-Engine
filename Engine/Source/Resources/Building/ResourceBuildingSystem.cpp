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
#include <Systems/PhysicsSystem.h>
#include <Systems/TaskSystem.h>

//Third party.
#include <ThirdParty/ft2build.h>
#include <ThirdParty/freetype/freetype.h>
#include <ThirdParty/stb_image.h>
#include <ThirdParty/stb_image_resize.h>
#include <ThirdParty/glslang/Public/ShaderLang.h>

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
	constexpr uint32 INTERNAL_FONT_RESOLUTION{ 1'024 };
	constexpr int32 PADDING_BETWEEN_CHARACTERS{ INTERNAL_FONT_RESOLUTION / 4 };
	constexpr uint8 BASE_MIP_LEVEL{ 4 };
	constexpr uint8 EXTRA_MIP_LEVELS{ 3 };
	constexpr uint8 MIP_CHAIN_LEVELS{ BASE_MIP_LEVEL + 1 + EXTRA_MIP_LEVELS };
	constexpr uint8 TOTAL_NUMBER_OF_MIPMAP_LEVELS{ 1 + EXTRA_MIP_LEVELS };

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::FONT_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Initialize the FreeType library.
	FT_Library free_type_library;

	if (FT_Init_FreeType(&free_type_library))
	{
		ASSERT(false, "Failed to initialie the FreeType library!");
	}

	//Load the face.
	FT_Face free_type_face;

	if (FT_New_Face(free_type_library, parameters._File, 0, &free_type_face))
	{
		ASSERT(false, "Failed to load the FreeType face!");
	}

	//Set the maximum font size.
	FT_Set_Pixel_Sizes(free_type_face, 0, INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);

	//Determine the width of the master texture.
	uint32 master_texture_width{ 0 };

	for (int8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		if (FT_Load_Char(free_type_face, i, FT_LOAD_RENDER))
		{
			ASSERT(false, "Failed to load the FreeType character!");
		}

		ASSERT(free_type_face->glyph->bitmap.width <= INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS && free_type_face->glyph->bitmap.rows <= INTERNAL_FONT_RESOLUTION  - PADDING_BETWEEN_CHARACTERS, "Something went wrong here!");

		const Vector2<uint32> character_dimensions{ free_type_face->glyph->bitmap.width, free_type_face->glyph->bitmap.rows };

		//Skip this character the dimensions are zero.
		if (character_dimensions._X == 0 || character_dimensions._Y == 0)
		{
			master_texture_width += PADDING_BETWEEN_CHARACTERS;

			continue;
		}

		master_texture_width += free_type_face->glyph->bitmap.width + PADDING_BETWEEN_CHARACTERS + PADDING_BETWEEN_CHARACTERS;
	}

	//Round the master texture width up to the nearest power of 2.
	master_texture_width = CatalystBaseMath::RoundUpToNearestPowerOfTwo(master_texture_width);

	//Initialize the master texture.
	Texture2D<float32> master_texture{ master_texture_width, INTERNAL_FONT_RESOLUTION };
	uint32 current_width_offset{ PADDING_BETWEEN_CHARACTERS / 2 };

	Memory::Set(master_texture.Data(), 0, master_texture.GetWidth() * master_texture.GetHeight());

	//Load all characters.
	for (int8 i{ 0 }; i < FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS; ++i)
	{
		PRINT_TO_OUTPUT("Calculating character " << static_cast<int32>(i + 1) << " of " << static_cast<int32>(FontResource::NUMBER_OF_CHARACTER_DESCRIPTIONS));

		if (FT_Load_Char(free_type_face, i, FT_LOAD_RENDER))
		{
			ASSERT(false, "Failed to load the FreeType character!");
		}

		//Write the character description to the file.
		FontResource::CharacterDescription character_description;

		character_description._Size._X = static_cast<float32>(free_type_face->glyph->bitmap.width + PADDING_BETWEEN_CHARACTERS) / static_cast<float32>(INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);
		character_description._Size._Y = static_cast<float32>(free_type_face->glyph->bitmap.rows + PADDING_BETWEEN_CHARACTERS) / static_cast<float32>(INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);
		character_description._Bearing._X = static_cast<float32>(free_type_face->glyph->bitmap_left) / static_cast<float32>(INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);
		character_description._Bearing._Y = static_cast<float32>(free_type_face->glyph->bitmap_top) / static_cast<float32>(INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);
		character_description._Advance = static_cast<float32>(free_type_face->glyph->advance.x >> 6) / static_cast<float32>(INTERNAL_FONT_RESOLUTION - PADDING_BETWEEN_CHARACTERS);
		character_description._TextureWidthOffsetStart = static_cast<float32>(current_width_offset) / static_cast<float32>(master_texture_width);
		character_description._TextureWidthOffsetEnd = static_cast<float32>(current_width_offset + PADDING_BETWEEN_CHARACTERS / 2 + free_type_face->glyph->bitmap.width + PADDING_BETWEEN_CHARACTERS / 2) / static_cast<float32>(master_texture_width);

		file.Write(&character_description, sizeof(FontResource::CharacterDescription));

		//Write the character dimensions.
		const Vector2<uint32> character_dimensions{ free_type_face->glyph->bitmap.width, free_type_face->glyph->bitmap.rows };

		//Skip this character the dimensions are zero.
		if (character_dimensions._X == 0 || character_dimensions._Y == 0)
		{
			current_width_offset += PADDING_BETWEEN_CHARACTERS;

			continue;
		}

		//Find the signed distance for all pixels.
		for (int32 Y{ 0 }; Y < static_cast<int32>(character_dimensions._Y) + PADDING_BETWEEN_CHARACTERS; ++Y)
		{
			for (int32 X{ 0 }; X < static_cast<int32>(character_dimensions._X) + PADDING_BETWEEN_CHARACTERS; ++X)
			{
				//Calculate the coordinate for the glyph buffer.
				const Vector2<int32> glyph_coordinate{ X - PADDING_BETWEEN_CHARACTERS / 2, Y - PADDING_BETWEEN_CHARACTERS / 2 };

				//Find the current pixel value.
				uint8 current_pixel_value;

				//If the sub coordinate is out of range, the pixel is assumed to be black.
				if (glyph_coordinate._X < 0
					|| static_cast<uint32>(glyph_coordinate._X) >= character_dimensions._X
					|| glyph_coordinate._Y < 0
					|| static_cast<uint32>(glyph_coordinate._Y) >= character_dimensions._Y)
				{
					current_pixel_value = 0;
				}

				else
				{
					current_pixel_value = free_type_face->glyph->bitmap.buffer[glyph_coordinate._X + (glyph_coordinate._Y * character_dimensions._X)] > 0 ? static_cast<uint8>(1) : static_cast<uint8>(0);
				}

				//Find the closest distance to the pixel with an opposite value of the one currently at.
				float32 closest_distance{ FLOAT32_MAXIMUM };

				for (int32 sub_y{ Y - (PADDING_BETWEEN_CHARACTERS / 2) }; sub_y < Y + (PADDING_BETWEEN_CHARACTERS / 2); ++sub_y)
				{
					for (int32 sub_x{ X - (PADDING_BETWEEN_CHARACTERS / 2) }; sub_x < X + (PADDING_BETWEEN_CHARACTERS / 2); ++sub_x)
					{
						//Calculate the coordinate for the glyph buffer.
						const Vector2<int32> glyph_coordinate{ sub_x - PADDING_BETWEEN_CHARACTERS / 2, sub_y - PADDING_BETWEEN_CHARACTERS / 2 };

						//Retrieve the value for the sample pixel.
						uint8 sample_pixel_value;

						if (glyph_coordinate._X < 0
							|| static_cast<uint32>(glyph_coordinate._X) >= character_dimensions._X
							|| glyph_coordinate._Y < 0
							|| static_cast<uint32>(glyph_coordinate._Y) >= character_dimensions._Y)
						{
							sample_pixel_value = 0;
						}

						else
						{
							sample_pixel_value = free_type_face->glyph->bitmap.buffer[glyph_coordinate._X + (glyph_coordinate._Y * character_dimensions._X)] > 0 ? static_cast<uint8>(1) : static_cast<uint8>(0);
						}

						//If the sample pixel value is different from the current pixel value, test it's length.
						if (current_pixel_value != sample_pixel_value)
						{
							const float32 distance_to_sample_pixel{ Vector2<float32>::LengthSquared(Vector2<float32>(static_cast<float32>(sub_x), static_cast<float32>(sub_y)) - Vector2<float32>(static_cast<float32>(X), static_cast<float32>(Y))) };

							if (closest_distance >= distance_to_sample_pixel)
							{
								closest_distance = distance_to_sample_pixel;
							}
						}
					}
				}

				//Perform the square root on the distance.
				closest_distance = CatalystBaseMath::SquareRoot(closest_distance);

				//Negate the distance if the current pixel value is inside the character.
				closest_distance = current_pixel_value == 1 ? -closest_distance : closest_distance;

				//Normalize the closest distance.
				closest_distance = 1.0f - (CatalystBaseMath::Clamp<float32>(closest_distance / static_cast<float32>(PADDING_BETWEEN_CHARACTERS / 2), -1.0f, 1.0f) * 0.5f + 0.5f);

				//Write the normalized distance to the master texture.
				master_texture.At(X + current_width_offset, Y) = closest_distance;
			}
		}

		current_width_offset += PADDING_BETWEEN_CHARACTERS / 2 + free_type_face->glyph->bitmap.width + PADDING_BETWEEN_CHARACTERS;
	}

	//Generate the mip chain for the final master textures.
	DynamicArray<Texture2D<float32>> final_master_textures;

	RenderingUtilities::GenerateMipChain(master_texture, MIP_CHAIN_LEVELS, &final_master_textures);

	//Convert the mip chain into byte textures.
	DynamicArray<Texture2D<byte>> final_final_master_textures;

	final_final_master_textures.Upsize<true>(MIP_CHAIN_LEVELS);

	for (uint8 i{ 0 }; i < MIP_CHAIN_LEVELS; ++i)
	{
		final_final_master_textures[i].Initialize(final_master_textures[i].GetWidth(), final_master_textures[i].GetHeight());

		for (uint32 Y{ 0 }; Y < final_final_master_textures[i].GetHeight(); ++Y)
		{
			for (uint32 X{ 0 }; X < final_final_master_textures[i].GetWidth(); ++X)
			{
				//Retrieve the value at this texel.
				final_final_master_textures[i].At(X, Y) = static_cast<uint8>(final_master_textures[i].At(X, Y) * static_cast<float32>(UINT8_MAXIMUM));
			}
		}
	}

	//Write the master texture dimensions to the file.
	const uint32 final_master_texture_width{ final_final_master_textures[BASE_MIP_LEVEL].GetWidth() };
	file.Write(&final_master_texture_width, sizeof(uint32));
	const uint32 final_master_texture_height{ final_final_master_textures[BASE_MIP_LEVEL].GetHeight() };
	file.Write(&final_master_texture_height, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	file.Write(&TOTAL_NUMBER_OF_MIPMAP_LEVELS, sizeof(uint8));

	//Write all the mip levels of the master texture to the file.
	for (uint8 i{ 0 }; i < TOTAL_NUMBER_OF_MIPMAP_LEVELS; ++i)
	{
		//Write the master texture data to the file.
		file.Write(final_final_master_textures[BASE_MIP_LEVEL + i].Data(), final_final_master_textures[BASE_MIP_LEVEL + i].GetWidth() * final_final_master_textures[BASE_MIP_LEVEL + i].GetHeight());
	}

	//Free FreeType's resources.
	FT_Done_Face(free_type_face);
	FT_Done_FreeType(free_type_library);

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

	//Read all model files into memory.
	DynamicArray<ModelFile> model_files;
	model_files.Upsize<true>(parameters._LevelOfDetails.Size());

	for (uint64 i{ 0 }, size{ model_files.Size() }; i < size; ++i)
	{
		if (parameters._ProceduralFunction)
		{
			parameters._ProceduralFunction(i, &model_files[i]);
		}
		
		else
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
		AxisAlignedBoundingBox3D aixs_aligned_bounding_box;

		for (ModelFile &model_file : model_files)
		{
			for (ModelFile::Mesh &mesh : model_file._Meshes)
			{
				for (Vertex &vertex : mesh._Vertices)
				{
					aixs_aligned_bounding_box.Expand(vertex._Position);
				}
			}
		}

		//Write the axis-aligned bounding box to the file.
		output_file.Write(&aixs_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3D));
	}

	//Write the number of meshes.
	const uint64 number_of_meshes{ model_files[0]._Meshes.Size() };
	ASSERT(number_of_meshes <= RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL, "This model has more then the maximum number of meshes, either increase the maximum number of meshes or decrease the number of meshes on this model!");
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

	//Check if there exists a collision model.
	if (parameters._CollisionModelFilePath)
	{
		//Write that there exists a collision model.
		bool collision_model_exists{ true };
		output_file.Write(&collision_model_exists, sizeof(bool));

		//Read the model file.
		ModelFile collision_model_file;
		FBXReader::Read(parameters._CollisionModelFilePath, &collision_model_file);

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
	BinaryFile<IOMode::Out> file{ file_name.Data() };

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
	constexpr uint8 MIPMAP_LEVELS{ 7 };
	constexpr uint32 BASE_RESOLUTION{ 512 };

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

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
	TextureCube base_texture;
	base_texture.Initialize(BASE_RESOLUTION);

	for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
	{
		for (uint32 Y{ 0 }; Y < BASE_RESOLUTION; ++Y)
		{
			for (uint32 X{ 0 }; X < BASE_RESOLUTION; ++X)
			{
				//Define constants.
				constexpr Vector2<float32> INVERSE_ATAN{ 0.1591f, 0.3183f };

				//Calculate the direction
				Vector3<float32> direction;

				const float32 x_weight{ static_cast<float32>(X) / static_cast<float32>(BASE_RESOLUTION) };
				const float32 y_weight{ static_cast<float32>(Y) / static_cast<float32>(BASE_RESOLUTION) };

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

	//Create the mip chain.
	StaticArray<TextureCube, MIPMAP_LEVELS - 1> mip_chain;

	for (uint8 mipmap_level{ 0 }; mipmap_level < MIPMAP_LEVELS - 1; ++mipmap_level)
	{
		//Calculate the mip resolution.
		const uint32 mip_resolution{ BASE_RESOLUTION >> (mipmap_level + 1) };

		//Initialize the mip texture.
		mip_chain[mipmap_level].Initialize(mip_resolution);

		//Calculate the sample delta.
		const Vector2<float32> sample_delta{ 1.0f / static_cast<float32>(BASE_RESOLUTION), 1.0f / static_cast<float32>(BASE_RESOLUTION) };

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
	file.Write(&BASE_RESOLUTION, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	file.Write(&MIPMAP_LEVELS, sizeof(uint8));

	//Write the data to the file.
	for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
	{
		file.Write(base_texture.Face(face_index).Data(), BASE_RESOLUTION * BASE_RESOLUTION * sizeof(Vector4<float32>));
	}

	for (uint8 mipmap_level{ 0 }; mipmap_level < MIPMAP_LEVELS - 1; ++mipmap_level)
	{
		for (uint8 face_index{ 0 }; face_index < 6; ++face_index)
		{
			file.Write(mip_chain[mipmap_level].Face(face_index).Data(), (BASE_RESOLUTION >> (mipmap_level + 1)) * (BASE_RESOLUTION >> (mipmap_level + 1)) * sizeof(Vector4<float32>));
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
					texel[i] = input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].At(X, Y)[UNDERLYING(parameters._ChannelMappings[i]._Channel)];
				}

				else
				{
					texel[i] = parameters._Default[UNDERLYING(parameters._ChannelMappings[i]._Channel)];
				}
			}

			//Apply gamma correction, if desired.
			if (parameters._ApplyGammaCorrection)
			{
				for (uint8 i{ 0 }; i < 3; ++i)
				{
					texel[i] = powf(texel[i], 2.2f);
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

	//Generate the mip chain.
	DynamicArray<Texture2D<Vector4<float32>>> mip_chain;

	RenderingUtilities::GenerateMipChain(composite_texture,	parameters._BaseMipmapLevel + parameters._MipmapLevels, &mip_chain);

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

#if 0 //Print average color.
	//Calculate the average color.
	Vector4<float32> average_color{ 0.0f, 0.0f, 0.0f, 0.0f };

	for (uint32 Y{ 0 }; Y < mip_chain[parameters._BaseMipmapLevel].GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < mip_chain[parameters._BaseMipmapLevel].GetWidth(); ++X)
		{
			average_color += mip_chain[parameters._BaseMipmapLevel].At(X, Y);
		}
	}

	average_color /= static_cast<float32>(mip_chain[parameters._BaseMipmapLevel].GetWidth() * mip_chain[parameters._BaseMipmapLevel].GetHeight());

	PRINT_TO_OUTPUT("Average color for " << parameters._ID << ": " << average_color._R << "f, " << average_color._G << "f, " << average_color._B << "f, " << average_color._A << "f");
#endif

	//What should the file be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<BinaryFileMode::OUT> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_2D_TYPE_IDENTIFIER, HashString(parameters._ID), parameters._ID };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the number of mipmap levels to the file.
	file.Write(&parameters._MipmapLevels, sizeof(uint8));

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