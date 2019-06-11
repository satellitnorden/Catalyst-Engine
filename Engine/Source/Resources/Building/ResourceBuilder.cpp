//Header file.
#include <Resources/Building/ResourceBuilder.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox.h>

//Resources
#include <Resources/Building/AssimpBuilder.h>
#include <Resources/Core/ResourcesCore.h>

//Rendering.
#include <Rendering/Native/Font.h>
#include <Rendering/Native/Texture2D.h>
#include <Rendering/Native/Vertex.h>

//Third party.
#include <ThirdParty/ft2build.h>
#include FT_FREETYPE_H
#include <ThirdParty/stb_image.h>
#include <ThirdParty/stb_image_resize.h>

/*
*	Builds a resource collection.
*/
void ResourceBuilder::BuildResourceCollection(const ResourceCollectionBuildParameters &parameters) NOEXCEPT
{
	//What should the collection be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".crc";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the number of resources in the resource collection.
	const uint64 numberOfResources{ parameters._Resources.Size() };
	file.Write(&numberOfResources, sizeof(uint64));

	//Slam all resources into the resource collection.
	for (const char *RESTRICT resource : parameters._Resources)
	{
		//Open the resource file.
		BinaryFile<IOMode::In> resourceFile{ resource };

		//Get the size of the resource file.
		const uint64 resourceFileSize{ resourceFile.Size() };

		//Read the data in the resource file.
		void *RESTRICT resourceFileData = Memory::AllocateMemory(resourceFileSize);
		resourceFile.Read(resourceFileData, resourceFileSize);

		//Write the resource file data to the resource collection file.
		file.Write(resourceFileData, resourceFileSize);

		//Free the resource file data.
		Memory::FreeMemory(resourceFileData);

		//Close the resource file.
		resourceFile.Close();
	}

	//Close the file.
	file.Close();
}

/*
*	Builds a font.
*/
void ResourceBuilder::BuildFont(const FontBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::Font) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Write the number of mipmap levels to the file.
	file.Write(&parameters._MipmapLevels, sizeof(uint8));

	//Initialize the FreeType library.
	FT_Library freeTypeLibrary;

	if (FT_Init_FreeType(&freeTypeLibrary))
	{
		ASSERT(false, "Failed to initialie the FreeType library!");
	}

	//Load the face.
	FT_Face freeTypeFace;

	if (FT_New_Face(freeTypeLibrary, parameters._File, 0, &freeTypeFace))
	{
		ASSERT(false, "Failed to load the FreeType face!");
	}

	//Set the maximum font size.
	FT_Set_Pixel_Sizes(freeTypeFace, 0, parameters._MaximumFontResolution);

	//Load all characters.
	for (int8 i{ 0 }; i < INT8_MAXIMUM; ++i)
	{
		if (FT_Load_Char(freeTypeFace, i, FT_LOAD_RENDER))
		{
			ASSERT(false, "Failed to load the FreeType character!");
		}

		//Write the character description to the file.
		Font::CharacterDescription characterDescription;

		characterDescription._TextureIndex = UINT32_MAXIMUM;
		characterDescription._Size._X = static_cast<float>(freeTypeFace->glyph->bitmap.width) / static_cast<float>(parameters._MaximumFontResolution);
		characterDescription._Size._Y = static_cast<float>(freeTypeFace->glyph->bitmap.rows) / static_cast<float>(parameters._MaximumFontResolution);
		characterDescription._Bearing._X = static_cast<float>(freeTypeFace->glyph->bitmap_left) / static_cast<float>(parameters._MaximumFontResolution);
		characterDescription._Bearing._Y = static_cast<float>(freeTypeFace->glyph->bitmap_top) / static_cast<float>(parameters._MaximumFontResolution);
		characterDescription._Advance = static_cast<float>(freeTypeFace->glyph->advance.x >> 6) / static_cast<float>(parameters._MaximumFontResolution);

		file.Write(&characterDescription, sizeof(Font::CharacterDescription));

		//Write the character dimensions.
		const Vector2<uint32> characterDimensions{ freeTypeFace->glyph->bitmap.width, freeTypeFace->glyph->bitmap.rows };
		file.Write(&characterDimensions, sizeof(Vector2<float>));

		//Write the texture data for this character to the file.
		if (characterDimensions._X == 0 || characterDimensions._Y == 0)
		{
			continue;
		}

		for (uint8 i{ 0 }; i < parameters._MipmapLevels; ++i)
		{
			//If this is the first mip level, just copy the data.
			if (i == 0)
			{
				file.Write(freeTypeFace->glyph->bitmap.buffer, freeTypeFace->glyph->bitmap.width * freeTypeFace->glyph->bitmap.rows);
			}

			//Else, the image data should be resized.
			else
			{
				const uint32 downsampledWidth{ characterDimensions._X / (1 << i) };
				const uint32 downsampledHeight{ characterDimensions._Y / (1 << i) };
				const uint64 downsampledSize{ downsampledWidth * downsampledHeight };

				byte *RESTRICT downsampledData{ static_cast<byte *RESTRICT>(Memory::AllocateMemory(downsampledSize)) };
				stbir_resize_uint8(freeTypeFace->glyph->bitmap.buffer, freeTypeFace->glyph->bitmap.width, freeTypeFace->glyph->bitmap.rows, 0, downsampledData, downsampledWidth, downsampledWidth, 0, 1);

				file.Write(downsampledData, downsampledSize);

				Memory::FreeMemory(downsampledData);
			}
		}
	}

	//Free FreeType's resources.
	FT_Done_Face(freeTypeFace);
	FT_Done_FreeType(freeTypeLibrary);

	//Close the file.
	file.Close();
}

/*
*	Builds a model.
*/
void ResourceBuilder::BuildModel(const ModelBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::Model) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Build the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;

	AssimpBuilder::BuildModel(parameters._File, &vertices, &indices);

	//Transform all vertices and simultaneously calculate the bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

	axisAlignedBoundingBox._Minimum = Vector3<float>(FLOAT_MAXIMUM, FLOAT_MAXIMUM, FLOAT_MAXIMUM);
	axisAlignedBoundingBox._Maximum = Vector3<float>(-FLOAT_MAXIMUM, -FLOAT_MAXIMUM, -FLOAT_MAXIMUM);

	for (Vertex &vertex : vertices)
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
	file.Write(&axisAlignedBoundingBox, sizeof(AxisAlignedBoundingBox));

	//Write the size of the vertices to the file.
	const uint64 sizeOfVertices{ vertices.Size() };
	file.Write(&sizeOfVertices, sizeof(uint64));

	//Write the vertices to the file.
	file.Write(vertices.Data(), sizeof(Vertex) * sizeOfVertices);

	//Write the size of the indices to the file.
	const uint64 sizeOfIndices{ indices.Size() };
	file.Write(&sizeOfIndices, sizeof(uint64));

	//Write the vertices to the file.
	file.Write(indices.Data(), sizeof(uint32) * sizeOfIndices);

	//Close the file.
	file.Close();
}

/*
*	Builds a sound bank.
*/
void ResourceBuilder::BuildSoundBank(const SoundBankBuildParameters &parameters) NOEXCEPT
{
	//What should the sound bank be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::SoundBank) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Read the sound bank file.
	BinaryFile<IOMode::In> soundBankFile{ parameters._File };

	//Write the sound bank size to the file.
	const uint64 soundBankSize{ soundBankFile.Size() };
	file.Write(&soundBankSize, sizeof(uint64));

	//Read the sound bank data.
	DynamicArray<byte> soundBankData;
	soundBankData.UpsizeFast(soundBankSize);
	
	soundBankFile.Read(soundBankData.Data(), soundBankSize);

	//Close the sound bank file.
	soundBankFile.Close();

	//Write the sound bank data to the file.
	file.Write(soundBankData.Data(), soundBankSize);

	//Close the file.
	file.Close();
}

/*
*	Builds a texture cube
*/
void ResourceBuilder::BuildTextureCube(const TextureCubeBuildParameters &parameters) NOEXCEPT
{
	//Define constants.
	constexpr Vector2<float> INVERSE_ATAN{ 0.1591f, 0.3183f };

	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr ResourceType resourceType{ ResourceType::TextureCube };
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
	Memory::CopyMemory(hdrTexture.Data(), data, width * height * 4 * sizeof(float));

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
void ResourceBuilder::BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.Data() };

	//Write the resource type to the file.
	constexpr ResourceType resourceType{ ResourceType::Texture2D };
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
					byte *RESTRICT downsampledData{ static_cast<byte *RESTRICT>(Memory::AllocateMemory(textureSize)) };
					stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

					file.Write(downsampledData, textureSize);

					Memory::FreeMemory(downsampledData);
				}
			}

			//Free the texture data.
			stbi_image_free(data);

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
					byte *RESTRICT downsampledDataR{ dataR ? static_cast<byte *RESTRICT>(Memory::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataG{ dataG ? static_cast<byte *RESTRICT>(Memory::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataB{ dataB ? static_cast<byte *RESTRICT>(Memory::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataA{ dataA ? static_cast<byte *RESTRICT>(Memory::AllocateMemory(textureSize * 4)) : nullptr };

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

					Memory::FreeMemory(downsampledDataR);
					Memory::FreeMemory(downsampledDataG);
					Memory::FreeMemory(downsampledDataB);
					Memory::FreeMemory(downsampledDataA);
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