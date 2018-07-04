#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>
#include <Engine Core/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Resource creator.
#include <stb_image.h>
#include <stb_image_resize.h>

#define NUMBER_OF_TERRAIN_LAYERS 5

namespace TerrainMaterialCreator
{

	void CreateTerrainMaterial(const char *const RESTRICT arguments[]) NOEXCEPT
	{
		//What should the material be called?
		DynamicString terrainMaterialName{ arguments[0] };
		terrainMaterialName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> terrainMaterialFile{ terrainMaterialName.CString() };

		//Write the resource type to the file.
		constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::TerrainMaterial) };
		terrainMaterialFile.Write(&resourceType, sizeof(ResourceType));
	
		//Write the resource ID to the file.
		const HashString resourceID{ arguments[1] };
		terrainMaterialFile.Write(&resourceID, sizeof(HashString));

		//Determine how many mipmap levels that should be generated.
		const uint8 numberOfMipmapLevels{ static_cast<uint8>(*arguments[2] - '0') };

		//Write the number of mipmap levels to the file, to be read into a uint8.
		terrainMaterialFile.Write(&numberOfMipmapLevels, sizeof(uint8));

		for (uint8 i = 0; i < NUMBER_OF_TERRAIN_LAYERS; ++i)
		{
			//Load the layer albedo.
			int32 width, height, numberOfChannels;
			byte *data{ stbi_load(arguments[3 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			const uint32 uWidth{ static_cast<uint32>(width) };
			const uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the albedo into the file, to be read into uint32's.
			terrainMaterialFile.Write(&uWidth, sizeof(uint32));
			terrainMaterialFile.Write(&uHeight, sizeof(uint32));

			//Write the layer albedo to the file, to be read into byte's.
			for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) * 4 };

				//If this is the base mipmap level, just copy the thing directly into memory.
				if (i == 0)
				{
					terrainMaterialFile.Write(data, textureSize);
				}

				//Else, the image data should be resized.
				else
				{
					byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
					stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

					terrainMaterialFile.Write(downsampledData, textureSize);

					MemoryUtilities::FreeMemory(downsampledData);
				}
			}

			//Free the layer albedo data.
			stbi_image_free(data);

			//Load the layer normal map.
			data = stbi_load(arguments[4 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the layer albedo to the file, to be read into byte's.
			for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) * 4 };

				//If this is the base mipmap level, just copy the thing directly into memory.
				if (i == 0)
				{
					terrainMaterialFile.Write(data, textureSize);
				}

				//Else, the image data should be resized.
				else
				{
					byte *RESTRICT downsampledData = static_cast<byte *RESTRICT>(MemoryUtilities::AllocateMemory(textureSize));
					stbir_resize_uint8(data, width, height, 0, downsampledData, uWidth >> i, uHeight >> i, 0, 4);

					terrainMaterialFile.Write(downsampledData, textureSize);

					MemoryUtilities::FreeMemory(downsampledData);
				}
			}

			//Free the layer normal map data.
			stbi_image_free(data);

			//Load the roughness, metallic, ambient occlusion and displacement data.
			unsigned char *roughnessData = stbi_load(arguments[5 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);
			unsigned char *metallicData = stbi_load(arguments[6 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);
			unsigned char *ambientOcclusionData = stbi_load(arguments[7 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);
			unsigned char *displacementData = stbi_load(arguments[8 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the roughness, metallic, ambient occlusion and displacement data to the file.
			unsigned char defaultRoughness{ 255 };
			unsigned char defaultMetallic{ 0 };
			unsigned char defaultAmbientOcclusion{ 255 };
			unsigned char defaultDisplacement{ 0 };

			for (uint8 i = 0; i < numberOfMipmapLevels; ++i)
			{
				const uint64 textureSize{ (uWidth >> i) * (uHeight >> i) };

				//If this is the base mipmap level, treat it differently.
				if (i == 0)
				{
					for (uint64 j = 0; j < textureSize; ++j)
					{
						terrainMaterialFile.Write(roughnessData ? &roughnessData[j * 4] : &defaultRoughness, sizeof(char));
						terrainMaterialFile.Write(metallicData ? &metallicData[j * 4] : &defaultMetallic, sizeof(char));
						terrainMaterialFile.Write(ambientOcclusionData ? &ambientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(char));
						terrainMaterialFile.Write(displacementData ? &displacementData[j * 4] : &defaultDisplacement, sizeof(char));
					}
				}

				else
				{
					byte *RESTRICT downsampledRoughnessData = roughnessData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
					byte *RESTRICT downsampledMetallicData = metallicData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
					byte *RESTRICT downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;
					byte *RESTRICT downsampledDisplacementData = displacementData ? static_cast<byte *RESTRICT >(MemoryUtilities::AllocateMemory(textureSize * 4)) : nullptr;

					if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, uWidth >> i, uHeight >> i, 0, 4);
					if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, uWidth >> i, uHeight >> i, 0, 4);
					if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, uWidth >> i, uHeight >> i, 0, 4);
					if (displacementData) stbir_resize_uint8(displacementData, width, height, 0, downsampledDisplacementData, uWidth >> i, uHeight >> i, 0, 4);

					for (uint64 j = 0; j < textureSize; ++j)
					{
						terrainMaterialFile.Write(downsampledRoughnessData ? &downsampledRoughnessData[j * 4] : &defaultRoughness, sizeof(byte));
						terrainMaterialFile.Write(downsampledMetallicData ? &downsampledMetallicData[j * 4] : &defaultMetallic, sizeof(byte));
						terrainMaterialFile.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j * 4] : &defaultAmbientOcclusion, sizeof(byte));
						terrainMaterialFile.Write(downsampledDisplacementData ? &downsampledDisplacementData[j * 4] : &defaultDisplacement, sizeof(byte));
					}

					MemoryUtilities::FreeMemory(downsampledRoughnessData);
					MemoryUtilities::FreeMemory(downsampledMetallicData);
					MemoryUtilities::FreeMemory(downsampledAmbientOcclusionData);
					MemoryUtilities::FreeMemory(downsampledDisplacementData);
				}
			}

			//Free the roughness, metallic, ambient occlusion and displacement data.
			stbi_image_free(roughnessData);
			stbi_image_free(metallicData);
			stbi_image_free(ambientOcclusionData);
			stbi_image_free(displacementData);
		}

		//Close the file
		terrainMaterialFile.Close();
	}

}