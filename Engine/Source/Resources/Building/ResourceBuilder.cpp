//Header file.
#include <Resources/Building/ResourceBuilder.h>

//Core.
#include <Core/Containers/DynamicArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/Building/AssimpBuilder.h>
#include <Resources/Core/ResourcesCore.h>

//Rendering.
#include <Rendering/Engine/Vertex.h>

//Third party.
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
	BinaryFile<IOMode::Out> file{ fileName.CString() };

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
		void *RESTRICT resourceFileData = MemoryUtilities::AllocateMemory(resourceFileSize);
		resourceFile.Read(resourceFileData, resourceFileSize);

		//Write the resource file data to the resource collection file.
		file.Write(resourceFileData, resourceFileSize);

		//Free the resource file data.
		MemoryUtilities::FreeMemory(resourceFileData);

		//Close the resource file.
		resourceFile.Close();
	}

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
	BinaryFile<IOMode::Out> file{ fileName.CString() };

	//Write the resource type to the file.
	constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::Model) };
	file.Write(&resourceType, sizeof(ResourceType));

	//Write the resource ID to the file.
	const HashString resourceID{ parameters._ID };
	file.Write(&resourceID, sizeof(HashString));

	//Build the model.
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32> indices;
	float extent{ 0.0f };

	AssimpBuilder::BuildModel(parameters._File, &vertices, &indices, &extent);

	//Write the extent to the file.
	file.Write(&extent, sizeof(float));

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
*	Builds a texture 2D.
*/
void ResourceBuilder::BuildTexture2D(const Texture2DBuildParameters &parameters) NOEXCEPT
{
	//What should the material be called?
	DynamicString fileName{ parameters._Output };
	fileName += ".cr";

	//Open the file to be written to.
	BinaryFile<IOMode::Out> file{ fileName.CString() };

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
					byte *RESTRICT downsampledData{ static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize)) };
					stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

					file.Write(downsampledData, textureSize);

					MemoryUtilities::FreeMemory(downsampledData);
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
					byte *RESTRICT downsampledDataR{ dataR ? static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataG{ dataG ? static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataB{ dataB ? static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr };
					byte *RESTRICT downsampledDataA{ dataA ? static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr };

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

					MemoryUtilities::FreeMemory(downsampledDataR);
					MemoryUtilities::FreeMemory(downsampledDataG);
					MemoryUtilities::FreeMemory(downsampledDataB);
					MemoryUtilities::FreeMemory(downsampledDataA);
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