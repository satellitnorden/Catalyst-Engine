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
#include <File/Readers/WAVReader.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Rendering.
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

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::ANIMATED_MODEL) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

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
	//Define constants.
	constexpr uint8 RESOURCE_TYPE{ static_cast<uint8>(ResourceType::ANIMATION) };

	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource type to the file.
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resource_ID{ parameters._ID };
	file.Write(&resource_ID, sizeof(HashString));

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
	//What should the material be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource type to the file.
	constexpr uint8 RESOURCE_TYPE{ static_cast<uint8>(ResourceType::FONT) };
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resource_id{ parameters._ID };
	file.Write(&resource_id, sizeof(HashString));

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
	FT_Set_Pixel_Sizes(free_type_face, 0, parameters._MaximumFontResolution);

	//Load all characters.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		if (FT_Load_Char(free_type_face, i, FT_LOAD_RENDER))
		{
			ASSERT(false, "Failed to load the FreeType character!");
		}

		//Write the character description to the file.
		FontResource::CharacterDescription character_description;

		character_description._TextureIndex = UINT32_MAXIMUM;
		character_description._Size._X = static_cast<float>(free_type_face->glyph->bitmap.width) / static_cast<float>(parameters._MaximumFontResolution);
		character_description._Size._Y = static_cast<float>(free_type_face->glyph->bitmap.rows) / static_cast<float>(parameters._MaximumFontResolution);
		character_description._Bearing._X = static_cast<float>(free_type_face->glyph->bitmap_left) / static_cast<float>(parameters._MaximumFontResolution);
		character_description._Bearing._Y = static_cast<float>(free_type_face->glyph->bitmap_top) / static_cast<float>(parameters._MaximumFontResolution);
		character_description._Advance = static_cast<float>(free_type_face->glyph->advance.x >> 6) / static_cast<float>(parameters._MaximumFontResolution);

		file.Write(&character_description, sizeof(FontResource::CharacterDescription));

		//Write the character dimensions.
		const Vector2<uint32> character_dimensions{ free_type_face->glyph->bitmap.width, free_type_face->glyph->bitmap.rows };

		if (free_type_face->glyph->bitmap.width > 128 || free_type_face->glyph->bitmap.rows > 128)
		{
			CRASH();
		}

		file.Write(&character_dimensions, sizeof(Vector2<uint32>));

		//Write the texture data for this character to the file.
		if (character_dimensions._X == 0 || character_dimensions._Y == 0)
		{
			continue;
		}

		//Write the bitmap data.
		file.Write(free_type_face->glyph->bitmap.buffer, free_type_face->glyph->bitmap.width * free_type_face->glyph->bitmap.rows);
	}

	//Free FreeType's resources.
	FT_Done_Face(free_type_face);
	FT_Done_FreeType(free_type_library);

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

	//Write the resource type to the file.
	constexpr uint8 RESOURCE_TYPE{ static_cast<uint8>(ResourceType::MODEL) };
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resource_ID{ parameters._ID };
	file.Write(&resource_ID, sizeof(HashString));

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

	//First, compile the shader.
	{
		//Create a temporary batch file that stores all commands.
		std::ofstream batch_file;

		batch_file.open("temporary_batch_file.bat", std::ios::out);

		batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
		batch_file << " -V ";
		batch_file << parameters._FilePath;
		batch_file << " -o ";
		batch_file << compiled_file_path.Data();

		batch_file.close();

		system("temporary_batch_file.bat");

		//Delete the temporary batch file.
		File::Delete("temporary_batch_file.bat");
	}

	//If the file exists, recreate the temporary batch file with a pause statement in the end to display the error.
	if (!File::Exists(compiled_file_path.Data()))
	{
		std::ofstream batch_file;

		batch_file.open("temporary_batch_file.bat", std::ios::out);

		batch_file << "C:\\Github\\Catalyst-Engine\\Engine\\Binaries\\glslangValidator.exe";
		batch_file << " -V ";
		batch_file << parameters._FilePath;
		batch_file << " -o ";
		batch_file << compiled_file_path.Data();
		batch_file << std::endl;
		batch_file << "pause";

		batch_file.close();

		system("temporary_batch_file.bat");
	}

	//What should the resource be called?
	DynamicString file_name{ parameters._Output };
	file_name += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ file_name.Data() };

	//Write the resource type to the file.
	constexpr uint8 RESOURCE_TYPE{ static_cast<uint8>(ResourceType::SHADER) };
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource identifier to the file.
	const HashString resource_identifier{ parameters._ID };
	file.Write(&resource_identifier, sizeof(HashString));

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
		//What should the resource be called?
		DynamicString file_name{ parameters._Output };
		file_name += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> file{ file_name.Data() };

		//Write the resource type to the file.
		constexpr uint8 RESOURCE_TYPE{ static_cast<uint8>(ResourceType::SOUND) };
		file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

		//Write the resource identifier to the file.
		const HashString resource_identifier{ parameters._ID };
		file.Write(&resource_identifier, sizeof(HashString));

		//Write the sample rate to the file.
		file.Write(&resource._SampleRate, sizeof(float32));

		//Write the number of channels to the file.
		file.Write(&resource._NumberOfChannels, sizeof(uint8));

		//Write the number of samples.
		const uint64 number_of_samples{ resource._Samples[0].Size() }; //Assume all channels have the same size.
		file.Write(&number_of_samples, sizeof(uint64));

		//Write the samples.
		for (const DynamicArray<float32>& channel : resource._Samples)
		{
			file.Write(channel.Data(), sizeof(float32) * number_of_samples);
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
	constexpr Vector2<float> INVERSE_ATAN{ 0.1591f, 0.3183f };

	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr ResourceType resourceType{ ResourceType::TEXTURE_CUBE };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Load the texture.
	int32 width, height, numberOfChannels;
	float *const RESTRICT data{ stbi_loadf(parameters._File, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

	//Wrap the data into a texture 2D for easier manipulation.
	Texture2D<Vector4<float>> hdrTexture{ static_cast<uint32>(width), static_cast<uint32>(height) };

	//Copy the data into the cpu texture.
	Memory::Copy(hdrTexture.Data(), data, width * height * 4 * sizeof(float));

	//Create the diffuse output textures.
	StaticArray<Texture2D<Vector4<float>>, 6> outputTextures
	{
		Texture2D<Vector4<float>>(parameters._Resolution),
		Texture2D<Vector4<float>>(parameters._Resolution),
		Texture2D<Vector4<float>>(parameters._Resolution),
		Texture2D<Vector4<float>>(parameters._Resolution),
		Texture2D<Vector4<float>>(parameters._Resolution),
		Texture2D<Vector4<float>>(parameters._Resolution)
	};

	for (uint8 i{ 0 }; i < 6; ++i)
	{
		for (uint32 j = 0; j < parameters._Resolution; ++j)
		{
			for (uint32 k = 0; k < parameters._Resolution; ++k)
			{
				Vector3<float> position;

				const float xWeight{ static_cast<float>(j) / static_cast<float>(parameters._Resolution) };
				const float yWeight{ static_cast<float>(k) / static_cast<float>(parameters._Resolution) };

				switch (i)
				{
					default: CRASH(); break;
					case 0: position = Vector3<float>(-1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight)); break; //Front.
					case 1: position = Vector3<float>(1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight)); break; //Back.
					case 2: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), -1.0f, CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, yWeight)); break; //Up.
					case 3: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), 1.0f, CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight)); break; //Down.
					case 4: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(1.0f, -1.0f, xWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), -1.0f); break; //Right.
					case 5: position = Vector3<float>(CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, xWeight), CatalystBaseMath::LinearlyInterpolate(-1.0f, 1.0f, yWeight), 1.0f); break; //Left.
				}

				position.Normalize();

				Vector2<float> textureCoordinate{ CatalystBaseMath::Arctangent(position._Z, position._X), CatalystBaseMath::Arcsine(position._Y) };
				textureCoordinate *= INVERSE_ATAN;
				textureCoordinate += 0.5f;

				outputTextures[i].At(j, k) = hdrTexture.Sample(textureCoordinate, AddressMode::ClampToEdge);
			}
		}
	}

	//Write the resolution to the file.
	file.Write(&parameters._Resolution, sizeof(uint32));

	//Write the diffuse to the file.
	for (uint8 i = 0; i < 6; ++i)
	{
		file.Write(outputTextures[i].Data(), parameters._Resolution * parameters._Resolution * 4 * sizeof(float));
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a texture 2D.
*/
void ResourceBuildingSystem::BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr ResourceType resourceType{ ResourceType::TEXTURE_2D };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Write the number of mipmap levels to the file.
	file.Write(&parameters._MipmapLevels, sizeof(uint8));

	switch (parameters._Mode)
	{
		case Texture2DBuildParameters::Mode::RToRGBA:
		{
			//Load the texture data.
			int32 width, height, numberOfChannels;
			byte *RESTRICT data{ stbi_load(parameters._FileR, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			const uint32 uWidth{ static_cast<uint32>(width) };
			const uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the texture to the file.
			file.Write(&uWidth, sizeof(uint32));
			file.Write(&uHeight, sizeof(uint32));

			//Apply gamma correction.
			if (parameters._ApplyGammaCorrection)
			{
				for (uint64 i{ 0 }; i < uWidth * uHeight * 4; ++i)
				{
					byte& texel{ data[i] };

					float texel_float{ static_cast<float>(texel) / 255.0f };
					texel_float = powf(texel_float, 2.2f);

					texel = static_cast<byte>(texel_float * 255.0f);
				}
			}

			//Write the texture to the file.
			for (uint8 i{ 0 }; i < parameters._MipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) * 4 };

				//If this is the base mipmap level, just copy the thing directly into memory.
				if (i == 0)
				{
					file.Write(data, textureSize);
				}

				//Else, the image data should be resized.
				else
				{
					byte *RESTRICT downsampledData{ static_cast<byte *RESTRICT>(Memory::Allocate(textureSize)) };
					stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

					file.Write(downsampledData, textureSize);

					Memory::Free(downsampledData);
				}
			}

			//Free the texture data.
			stbi_image_free(data);

			break;
		}

		case Texture2DBuildParameters::Mode::RToRGBAToA:
		{
			//Load the files for the R and A channels.
			int32 width, height, numberOfChannels;

			byte* RESTRICT dataR{ parameters._FileR ? stbi_load(parameters._FileR, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };
			byte* RESTRICT dataA{ parameters._FileA ? stbi_load(parameters._FileA, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };

			const uint32 uWidth{ static_cast<uint32>(width) };
			const uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the texture to the file.
			file.Write(&uWidth, sizeof(uint32));
			file.Write(&uHeight, sizeof(uint32));

			//Write the R and A channel data to the file.
			constexpr byte DEFAULT_R{ 255 };
			constexpr byte DEFAULT_A{ 255 };

			for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i)* (uHeight >> i) };

				//If this is the base mipmap level, treat it differently.
				if (i == 0)
				{
					for (uint64 j = 0; j < textureSize; ++j)
					{
						file.Write(dataR ? &dataR[j * 4 + 0] : &DEFAULT_R, sizeof(byte));
						file.Write(dataR ? &dataR[j * 4 + 1] : &DEFAULT_R, sizeof(byte));
						file.Write(dataR ? &dataR[j * 4 + 2] : &DEFAULT_R, sizeof(byte));
						file.Write(dataA ? &dataA[j * 4] : &DEFAULT_A, sizeof(byte));
					}
				}

				else
				{
					byte* RESTRICT downsampledDataR{ dataR ? static_cast<byte * RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };
					byte* RESTRICT downsampledDataA{ dataA ? static_cast<byte * RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };

					if (dataR) stbir_resize_uint8(dataR, width, height, 0, downsampledDataR, uWidth >> i, uHeight >> i, 0, 4);
					if (dataA) stbir_resize_uint8(dataA, width, height, 0, downsampledDataA, uWidth >> i, uHeight >> i, 0, 4);

					for (uint64 j = 0; j < textureSize; ++j)
					{
						file.Write(downsampledDataR ? &downsampledDataR[j * 4 + 0] : &DEFAULT_R, sizeof(byte));
						file.Write(downsampledDataR ? &downsampledDataR[j * 4 + 1] : &DEFAULT_R, sizeof(byte));
						file.Write(downsampledDataR ? &downsampledDataR[j * 4 + 2] : &DEFAULT_R, sizeof(byte));
						file.Write(downsampledDataA ? &downsampledDataA[j * 4] : &DEFAULT_A, sizeof(byte));
					}

					Memory::Free(downsampledDataR);
					Memory::Free(downsampledDataA);
				}
			}

			//Free the R and A channel data.
			stbi_image_free(dataR);
			stbi_image_free(dataA);

			//Close the file.
			file.Close();

			break;
		}

		case Texture2DBuildParameters::Mode::RToRGToGBToBAToA:
		{
			//Load the files for the R, G, B and A channels.
			int32 width, height, numberOfChannels;

			byte *RESTRICT dataR{ parameters._FileR ? stbi_load(parameters._FileR, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };
			byte *RESTRICT dataG{ parameters._FileG ? stbi_load(parameters._FileG, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };
			byte *RESTRICT dataB{ parameters._FileB ? stbi_load(parameters._FileB, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };
			byte *RESTRICT dataA{ parameters._FileA ? stbi_load(parameters._FileA, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr };

			const uint32 uWidth{ static_cast<uint32>(width) };
			const uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the texture to the file.
			file.Write(&uWidth, sizeof(uint32));
			file.Write(&uHeight, sizeof(uint32));

			//Write the R, G, B and A channel data to the file.
			constexpr byte DEFAULT_R{ 255 };
			constexpr byte DEFAULT_G{ 0 };
			constexpr byte DEFAULT_B{ 255 };
			constexpr byte DEFAULT_A{ 0 };

			for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) };

				//If this is the base mipmap level, treat it differently.
				if (i == 0)
				{
					for (uint64 j = 0; j < textureSize; ++j)
					{
						file.Write(dataR ? &dataR[j * 4] : &DEFAULT_R, sizeof(byte));
						file.Write(dataG ? &dataG[j * 4] : &DEFAULT_G, sizeof(byte));
						file.Write(dataB ? &dataB[j * 4] : &DEFAULT_B, sizeof(byte));
						file.Write(dataA ? &dataA[j * 4] : &DEFAULT_A, sizeof(byte));
					}
				}

				else
				{
					byte *RESTRICT downsampledDataR{ dataR ? static_cast<byte *RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataG{ dataG ? static_cast<byte *RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataB{ dataB ? static_cast<byte *RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataA{ dataA ? static_cast<byte *RESTRICT>(Memory::Allocate(textureSize * 4)) : nullptr };

					if (dataR) stbir_resize_uint8(dataR, width, height, 0, downsampledDataR, uWidth >> i, uHeight >> i, 0, 4);
					if (dataG) stbir_resize_uint8(dataG, width, height, 0, downsampledDataG, uWidth >> i, uHeight >> i, 0, 4);
					if (dataB) stbir_resize_uint8(dataB, width, height, 0, downsampledDataB, uWidth >> i, uHeight >> i, 0, 4);
					if (dataA) stbir_resize_uint8(dataA, width, height, 0, downsampledDataA, uWidth >> i, uHeight >> i, 0, 4);

					for (uint64 j = 0; j < textureSize; ++j)
					{
						file.Write(downsampledDataR ? &downsampledDataR[j * 4] : &DEFAULT_R, sizeof(byte));
						file.Write(downsampledDataG ? &downsampledDataG[j * 4] : &DEFAULT_G, sizeof(byte));
						file.Write(downsampledDataB ? &downsampledDataB[j * 4] : &DEFAULT_B, sizeof(byte));
						file.Write(downsampledDataA ? &downsampledDataA[j * 4] : &DEFAULT_A, sizeof(byte));
					}

					Memory::Free(downsampledDataR);
					Memory::Free(downsampledDataG);
					Memory::Free(downsampledDataB);
					Memory::Free(downsampledDataA);
				}
			}

			//Free the R, G, B and A channel data.
			stbi_image_free(dataR);
			stbi_image_free(dataG);
			stbi_image_free(dataB);
			stbi_image_free(dataA);

			//Close the file.
			file.Close();

			break;
		}

		default:
		{
			ASSERT(false, "Not implemented yet. ):");

			break;
		}
	}
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

	//Write the resource type to the file.
	constexpr ResourceType RESOURCE_TYPE{ ResourceType::TEXTURE_3D };
	file.Write(&RESOURCE_TYPE, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resource_ID{ parameters._ID };
	file.Write(&resource_ID, sizeof(HashString));

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