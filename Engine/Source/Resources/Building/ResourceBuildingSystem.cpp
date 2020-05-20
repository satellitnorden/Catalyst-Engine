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

//File.
#include <File/Core/FileCore.h>
#include <File/Core/BinaryFile.h>
#include <File/Readers/JPGReader.h>
#include <File/Readers/PNGReader.h>
#include <File/Readers/WAVReader.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/Vertex.h>

//Resources
#include <Resources/Building/AssimpBuilder.h>
#include <Resources/Core/FontResource.h>
#include <Resources/Core/ResourcesCore.h>

//Third party.
#include <ThirdParty/ft2build.h>
#include FT_FREETYPE_H
#include <ThirdParty/stb_image.h>
#include <ThirdParty/stb_image_resize.h>

/*
*	Builds a resource collection.
*/
void ResourceBuildingSystem::BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT
{
	//What should the collection be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".crc";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Count the number of resources in the folder.
	uint64 number_of_resources{ 0 };

	for (const auto &resource : std::filesystem::directory_iterator(std::string(parameters._Folder)))
	{
		++number_of_resources;
	}

	//Write the number of resources in the resource collection.
	file.Write(&number_of_resources, sizeof(uint64));

	//Iterate over all files in the folder in the folder and add them to the resource collection.
	for (const auto &resource : std::filesystem::directory_iterator(std::string(parameters._Folder)))
	{
		//Open the resource file.
		BinaryFile<IOMode::In> resourceFile{ resource.path().string().c_str() };

		//Get the size of the resource file.
		const uint64 resourceFileSize{ resourceFile.Size() };

		//Read the data in the resource file.
		void* RESTRICT resourceFileData = Memory::Allocate(resourceFileSize);
		resourceFile.Read(resourceFileData, resourceFileSize);

		//Write the resource file data to the resource collection file.
		file.Write(resourceFileData, resourceFileSize);

		//Free the resource file data.
		Memory::Free(resourceFileData);

		//Close the resource file.
		resourceFile.Close();
	}

	//Close the file.
	file.Close();
}

/*
*	Writes a bone to file.
*/
FORCE_INLINE void WriteBoneToFile(const Bone &bone, BinaryFile<IOMode::Out> *const RESTRICT file) NOEXCEPT
{
	file->Write(&bone, sizeof(Bone) - sizeof(DynamicArray<Bone>));

	const uint64 number_of_child_bones{ bone._Children.Size() };
	file->Write(&number_of_child_bones, sizeof(uint64));

	for (const Bone &child_bone : bone._Children)
	{
		WriteBoneToFile(child_bone, file);
	}
}

/*
*	Builds an animated model.
*/
void ResourceBuildingSystem::BuildAnimatedModel(const AnimatedModelBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::ANIMATED_MODEL_TYPE_IDENTIFIER, HashString(parameters._ID) };
	file.Write(&header, sizeof(ResourceHeader));

	//Build the model.
	DynamicArray<AnimatedVertex> vertices;
	DynamicArray<uint32> indices;
	Skeleton skeleton;

	AssimpBuilder::BuildAnimatedModel(parameters._File, &vertices, &indices, &skeleton);

	//Transform all vertices and simultaneously calculate the bounding box.
	AxisAlignedBoundingBox3 axisAlignedBoundingBox;

	axisAlignedBoundingBox._Minimum = Vector3<float>(FLOAT_MAXIMUM, FLOAT_MAXIMUM, FLOAT_MAXIMUM);
	axisAlignedBoundingBox._Maximum = Vector3<float>(-FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM);

	for (AnimatedVertex &vertex : vertices)
	{
		if (parameters._Transformation != MatrixConstants::IDENTITY || parameters._TexturCoordinateRotation != 0.0f)
		{
			vertex.Transform(parameters._Transformation, parameters._TexturCoordinateRotation);
		}

		axisAlignedBoundingBox._Minimum = Vector3<float>::Minimum(axisAlignedBoundingBox._Minimum, vertex._Position);
		axisAlignedBoundingBox._Maximum = Vector3<float>::Maximum(axisAlignedBoundingBox._Maximum, vertex._Position);

		vertex._TextureCoordinate *= parameters._TextureCoordinateMultiplier;
	}

	//Write the axis-aligned bounding box to the file.
	file.Write(&axisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox3));

	//Write the size of the vertices to the file.
	const uint64 sizeOfVertices{ vertices.Size() };
	file.Write(&sizeOfVertices, sizeof(uint64));

	//Write the vertices to the file.
	file.Write(vertices.Data(), sizeof(AnimatedVertex) * sizeOfVertices);

	//Write the size of the indices to the file.
	const uint64 sizeOfIndices{ indices.Size() };
	file.Write(&sizeOfIndices, sizeof(uint64));

	//Write the vertices to the file.
	file.Write(indices.Data(), sizeof(uint32) * sizeOfIndices);

	//Write all the bones to the file.
	WriteBoneToFile(skeleton._RootBone, &file);

	//Close the file.
	file.Close();
}

/*
*	Builds an animation.
*/
void ResourceBuildingSystem::BuildAnimation(const AnimationBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::ANIMATION_TYPE_IDENTIFIER, HashString(parameters._ID) };
	file.Write(&header, sizeof(ResourceHeader));

	//Build the animation.
	AnimationResource animation;
	AssimpBuilder::BuildAnimation(parameters._File, &animation);

	//Sort the animation keyframes.
	for (Pair<HashString, DynamicArray<AnimationKeyframe>>& keyframes : animation._Keyframes)
	{
		SortingAlgorithms::InsertionSort<AnimationKeyframe>
		(
			keyframes._Second.Begin(),
			keyframes._Second.End(),
			nullptr,
			[](const void* const RESTRICT, const AnimationKeyframe* const RESTRICT first, const AnimationKeyframe* const RESTRICT second)
			{
				return first->_Timestamp < second->_Timestamp;
			}
		);
	}

	//Write the duration of the animation.
	file.Write(&animation._Duration, sizeof(float));

	//Write the number of animation keyframe channels.
	const uint64 number_of_animation_keyframe_channels{ animation._Keyframes.Size() };
	file.Write(&number_of_animation_keyframe_channels, sizeof(uint64));

	//Write the animation keyframes.
	for (Pair<HashString, DynamicArray<AnimationKeyframe>>& keyframes : animation._Keyframes)
	{
		//Write the name of the channel.
		file.Write(&keyframes._First, sizeof(HashString));

		//Write the number of animation keyframes.
		const uint64 number_of_animation_keyframes{ keyframes._Second.Size() };
		file.Write(&number_of_animation_keyframes, sizeof(uint64));

		//Write the keyframes.
		file.Write(keyframes._Second.Data(), sizeof(AnimationKeyframe) * number_of_animation_keyframes);
	}

	//Close the file.
	file.Close();
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
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::FONT_TYPE_IDENTIFIER, HashString(parameters._ID) };
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

	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
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
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		PRINT_TO_OUTPUT("Calculating character " << static_cast<int32>(i + 1) << " of " << static_cast<int32>(INT8_MAXIMUM));

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
*	Builds a material.
*/
void ResourceBuildingSystem::BuildMaterial(const MaterialBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::MATERIAL_TYPE_IDENTIFIER, HashString(parameters._ID) };
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
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::MODEL_TYPE_IDENTIFIER, HashString(parameters._ID) };
	file.Write(&header, sizeof(ResourceHeader));

	//Determine the model space axis aligned bounding box. Assume the first level of detail will be representative in this sense for the rest level of details.
	{
		//Build the model.
		DynamicArray<DynamicArray<Vertex>> vertices;
		DynamicArray<DynamicArray<uint32>> indices;

		AssimpBuilder::BuildModel(parameters._LevelOfDetails[0], &vertices, &indices);

		//Transform all vertices and simultaneously calculate the bounding box.
		AxisAlignedBoundingBox3 aixs_aligned_bounding_box;

		aixs_aligned_bounding_box._Minimum = Vector3<float>(FLOAT_MAXIMUM, FLOAT_MAXIMUM, FLOAT_MAXIMUM);
		aixs_aligned_bounding_box._Maximum = Vector3<float>(-FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM);

		for (DynamicArray<Vertex>& mesh_vertices : vertices)
		{
			for (Vertex& vertex : mesh_vertices)
			{
				if (parameters._Transformation != MatrixConstants::IDENTITY || parameters._TexturCoordinateRotation != 0.0f)
				{
					vertex.Transform(parameters._Transformation, parameters._TexturCoordinateRotation);
				}

				aixs_aligned_bounding_box._Minimum = Vector3<float>::Minimum(aixs_aligned_bounding_box._Minimum, vertex._Position);
				aixs_aligned_bounding_box._Maximum = Vector3<float>::Maximum(aixs_aligned_bounding_box._Maximum, vertex._Position);
			}
		}

		//Write the axis-aligned bounding box to the file.
		file.Write(&aixs_aligned_bounding_box, sizeof(AxisAlignedBoundingBox3));
	}

	//Determine the number of meshes. Assume that each level of detail has the same number of meshes.
	const uint64 number_of_meshes{ AssimpBuilder::DetermineNumberOfMeshes(parameters._LevelOfDetails[0]) };

	ASSERT(number_of_meshes <= RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL, "This model has more then the maximum number of meshes, either increase the maximum number of meshes or decrease the number of meshes on this model!");

	//Write the number of meshes.
	file.Write(&number_of_meshes, sizeof(uint64));

	//Write the number of level of details.
	const uint64 number_of_level_of_details{ parameters._LevelOfDetails.Size() };
	file.Write(&number_of_level_of_details, sizeof(uint64));

	//Process each mesh individually.
	for (uint64 i{ 0 }; i < number_of_meshes; ++i)
	{
		for (const char* const RESTRICT level_of_detail : parameters._LevelOfDetails)
		{
			//Build the model.
			DynamicArray<DynamicArray<Vertex>> vertices;
			DynamicArray<DynamicArray<uint32>> indices;

			AssimpBuilder::BuildModel(level_of_detail, &vertices, &indices);

			//Transform all vertices for this mesh.
			for (Vertex& vertex : vertices[i])
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

			//Write the number of vertices to the file.
			const uint64 number_of_vertices{ vertices[i].Size() };
			file.Write(&number_of_vertices, sizeof(uint64));

			//Write the vertices to the file.
			file.Write(vertices[i].Data(), sizeof(Vertex) * number_of_vertices);

			//Write the number of indices to the file.
			const uint64 number_of_indices{ indices[i].Size() };
			file.Write(&number_of_indices, sizeof(uint64));

			//Write the vertices to the file.
			file.Write(indices[i].Data(), sizeof(uint32) * number_of_indices);
		}
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a shader.
*/
void ResourceBuildingSystem::BuildShader(const ShaderBuildParameters &parameters) NOEXCEPT
{
	//Determine the compiled file path.
	DynamicString compiled_file_path{ parameters._ID };
	compiled_file_path += ".compiled";

	//Remember the name of the temporary shader file path.
	std::string temporary_shader_file_path;

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

			//Determine the temporary shader file path.
			temporary_shader_file_path = parameters._FilePath + std::string(".glsl");

			//Write the compiler-ready version to a new temporary file.
			std::ofstream shader_file{ temporary_shader_file_path };

			shader_file << file_string;

			shader_file.close();
		}

		//Compile the file.
		{
			//Create a temporary batch file that stores all commands.
			std::ofstream batch_file;

			batch_file.open("temporary_batch_file.bat", std::ios::out);

			batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
			batch_file << " -V ";
			batch_file << temporary_shader_file_path;
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

			system("temporary_batch_file.bat");

			//Delete the temporary batch file.
			File::Delete("temporary_batch_file.bat");
		}
	}

	//If the file exists, recreate the temporary batch file with a pause statement in the end to display the error.
	if (!File::Exists(compiled_file_path.Data()))
	{
		std::ofstream batch_file;

		batch_file.open("temporary_batch_file.bat", std::ios::out);

		batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
		batch_file << " -V ";
		batch_file << temporary_shader_file_path;
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

		system("temporary_batch_file.bat");
	}

	//Delete the temporary shader file.
	File::Delete(temporary_shader_file_path.c_str());

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::SHADER_TYPE_IDENTIFIER, HashString(parameters._ID) };
	file.Write(&header, sizeof(ResourceHeader));

	//Write the stage.
	file.Write(&parameters._Stage, sizeof(ShaderStage));

	//Open the compiled file.
	BinaryFile<IOMode::In> compiled_file{ compiled_file_path.Data() };

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
		BinaryFile<IOMode::Out> file{ file_name.Data() };

		//Write the resource header to the file.
		const ResourceHeader header{ ResourceConstants::SOUND_TYPE_IDENTIFIER, HashString(parameters._ID) };
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
		ASSERT(false, "Couldn't build sound!");
	}
}

/*
*	Builds a texture cube
*/
void ResourceBuildingSystem::BuildTextureCube(const TextureCubeBuildParameters &parameters) NOEXCEPT
{
	//Define constants.
	constexpr Vector2<float32> INVERSE_ATAN{ 0.1591f, 0.3183f };

	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_CUBE_TYPE_IDENTIFIER, HashString(parameters._ID) };
	file.Write(&header, sizeof(ResourceHeader));

	//Load the texture.
	int32 width, height, number_of_channels;
	float32 *const RESTRICT data{ stbi_loadf(parameters._File, &width, &height, &number_of_channels, STBI_rgb_alpha) };

	//Wrap the data into a texture 2D for easier manipulation.
	Texture2D<Vector4<float32>> hdr_texture{ static_cast<uint32>(width), static_cast<uint32>(height) };

	//Copy the data into the cpu texture.
	Memory::Copy(hdr_texture.Data(), data, width * height * 4 * sizeof(float32));

	//Create the diffuse output textures.
	StaticArray<Texture2D<Vector4<float32>>, 6> output_textures
	{
		Texture2D<Vector4<float32>>(parameters._Resolution),
		Texture2D<Vector4<float32>>(parameters._Resolution),
		Texture2D<Vector4<float32>>(parameters._Resolution),
		Texture2D<Vector4<float32>>(parameters._Resolution),
		Texture2D<Vector4<float32>>(parameters._Resolution),
		Texture2D<Vector4<float32>>(parameters._Resolution)
	};

	for (uint8 i{ 0 }; i < 6; ++i)
	{
		for (uint32 j{ 0 }; j < parameters._Resolution; ++j)
		{
			for (uint32 k{ 0 }; k < parameters._Resolution; ++k)
			{
				Vector3<float32> position;

				const float32 x_weight{ static_cast<float32>(j) / static_cast<float32>(parameters._Resolution) };
				const float32 y_weight{ static_cast<float32>(k) / static_cast<float32>(parameters._Resolution) };

				switch (i)
				{
				default: CRASH(); break;
				case 0: position = Vector3<float>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight)); break; //Front.
				case 1: position = Vector3<float>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight)); break; //Back.
				case 2: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, y_weight)); break; //Up.
				case 3: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight)); break; //Down.
				case 4: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), -1.0f); break; //Right.
				case 5: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, x_weight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, y_weight), 1.0f); break; //Left.
				}

				position.Normalize();

				Vector2<float> texture_coordinate{ CatalystBaseMath::Arctangent(position._Z, position._X), CatalystBaseMath::Arcsine(position._Y) };
				texture_coordinate *= INVERSE_ATAN;
				texture_coordinate += 0.5f;

				output_textures[i].At(j, k) = hdr_texture.Sample(texture_coordinate, AddressMode::ClampToEdge);
			}
		}
	}

	//Write the resolution to the file.
	file.Write(&parameters._Resolution, sizeof(uint32));

	//Write the number of mipmap levels to the file.
	file.Write(&parameters._MipmapLevels, sizeof(uint8));

	//Create the mipmap chains.
	StaticArray<DynamicArray<Texture2D<Vector4<float32>>>, 6> mip_chains;

	for (uint8 i{ 0 }; i < 6; ++i)
	{
		RenderingUtilities::GenerateMipChain(output_textures[i], parameters._MipmapLevels, &mip_chains[i]);
	}

	//Write the data to the file.
	for (uint8 i{ 0 }; i < parameters._MipmapLevels; ++i)
	{
		for (uint8 j{ 0 }; j < 6; ++j)
		{
			file.Write(mip_chains[j][i].Data(), (parameters._Resolution >> i) * (parameters._Resolution >> i) * 4 * sizeof(float32));
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

	ASSERT(parameters._File1, "ResourceBuildingSystem::BuildTexture2D - Needs at least 1 input texuture!");

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

	//Create the composite texture. Assume that all input textures are the same size.
	Texture2D<Vector4<float32>> composite_texture{ input_textures[0].GetWidth(), input_textures[0].GetHeight() };

	for (uint32 Y{ 0 }; Y < composite_texture.GetHeight(); ++Y)
	{
		for (uint32 X{ 0 }; X < composite_texture.GetWidth(); ++X)
		{
			//Calculate the texel.
			Vector4<float32> &texel{ composite_texture.At(X, Y) };

			for (uint8 i{ 0 }; i < 4; ++i)
			{
				if (input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].GetWidth() > 0
					&& input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].GetHeight() > 0)
				{
					texel[i] = input_textures[UNDERLYING(parameters._ChannelMappings[i]._File)].At(X, Y)[UNDERLYING(parameters._ChannelMappings[i]._Channel)];
				}

				else
				{
					Vector4<float32> defaults{ 1.0f, 0.0f, 1.0f, 0.0f };
					texel[i] = defaults[i];
				}
			}

			//Apply gamma correction, if desired.
			if (parameters._ApplyGammaCorrection)
			{
				for (uint8 i{ 0 }; i < 4; ++i)
				{
					texel[i] = powf(texel[i], 2.2f);
				}
			}

			//Apply normal map strength, if desired.
			if (parameters._NormalMapStrength != 1.0f)
			{
				Vector3<float32> normal_map_direction{ texel._X * 2.0f - 1.0f, texel._Y * 2.0f - 1.0f, texel._Z * 2.0f - 1.0f };

				normal_map_direction._X *= parameters._NormalMapStrength;
				normal_map_direction._Y *= parameters._NormalMapStrength;

				normal_map_direction.Normalize();

				texel._X = normal_map_direction._X * 0.5f + 0.5f;
				texel._Y = normal_map_direction._Y * 0.5f + 0.5f;
				texel._Z = normal_map_direction._Z * 0.5f + 0.5f;
			}
		}
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

	//What should the file be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_2D_TYPE_IDENTIFIER, HashString(parameters._ID) };
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
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource header to the file.
	const ResourceHeader header{ ResourceConstants::TEXTURE_3D_TYPE_IDENTIFIER, HashString(parameters._ID) };
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
}
#endif