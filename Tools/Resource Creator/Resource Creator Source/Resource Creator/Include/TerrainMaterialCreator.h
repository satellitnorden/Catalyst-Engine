#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/DynamicString.h>
#include <Core/General/HashString.h>

//Resources
#include <Resources/ResourcesCore.h>

//Resource creator.
#include <stb_image.h>
#include <stb_image_resize.h>

namespace TerrainMaterialCreator
{
	//The number of terrain layers.
	static constexpr uint8 NUMBER_OF_TERRAIN_LAYERS{ 5 };

	class TerrainMaterialLayerInformation final
	{

	public:

		//The albedo file path.
		const char *RESTRICT _Albedo;

		//The normal map file path.
		const char *RESTRICT _Normal;

		//The roughness file path.
		const char *RESTRICT _Roughness;

		//The metallic file path.
		const char *RESTRICT _Metallic;

		//The ambient occlusion file path.
		const char *RESTRICT _AmbientOcclusion;

		//The displacement file path.
		const char *RESTRICT _Displacement;

	};

	class TerrainMaterialCreationParameters final
	{

	public:

		//The output file path.
		const char *RESTRICT _Output;

		//The resource id.
		const char *RESTRICT _ID;

		//The number of mipmap levels.
		uint8 _MipmapLevels;

		//The layer information.
		StaticArray<TerrainMaterialLayerInformation, NUMBER_OF_TERRAIN_LAYERS> _Layers;

	};

	void CreateTerrainMaterial(const TerrainMaterialCreationParameters &parameters) NOEXCEPT
	{
		//What should the material be called?
		DynamicString terrainMaterialName{ parameters._Output };
		terrainMaterialName += ".cr";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> terrainMaterialFile{ terrainMaterialName.CString() };

		//Write the resource type to the file.
		constexpr uint8 resourceType{ static_cast<uint8>(ResourceType::TerrainMaterial) };
		terrainMaterialFile.Write(&resourceType, sizeof(ResourceType));
	
		//Write the resource ID to the file.
		const HashString resourceID{ parameters._ID };
		terrainMaterialFile.Write(&resourceID, sizeof(HashString));

		//Write the number of mipmap levels to the file, to be read into a uint8.
		terrainMaterialFile.Write(&parameters._MipmapLevels, sizeof(uint8));

		for (uint8 i = 0; i < NUMBER_OF_TERRAIN_LAYERS; ++i)
		{
			//Load the layer albedo.
			int32 width, height, numberOfChannels;
			byte *data{ stbi_load(parameters._Layers[i]._Albedo, &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			const uint32 uWidth{ static_cast<uint32>(width) };
			const uint32 uHeight{ static_cast<uint32>(height) };

			//Write the width and height of the albedo into the file, to be read into uint32's.
			terrainMaterialFile.Write(&uWidth, sizeof(uint32));
			terrainMaterialFile.Write(&uHeight, sizeof(uint32));

			//Write the layer albedo to the file, to be read into byte's.
			for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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
			data = stbi_load(parameters._Layers[i]._Normal, &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the layer albedo to the file, to be read into byte's.
			for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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
			byte *roughnessData = parameters._Layers[i]._Roughness ? stbi_load(parameters._Layers[i]._Roughness, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
			byte *metallicData = parameters._Layers[i]._Metallic ? stbi_load(parameters._Layers[i]._Metallic, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
			byte *ambientOcclusionData = parameters._Layers[i]._AmbientOcclusion ? stbi_load(parameters._Layers[i]._AmbientOcclusion, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;
			byte *displacementData = parameters._Layers[i]._Displacement ? stbi_load(parameters._Layers[i]._Displacement, &width, &height, &numberOfChannels, STBI_rgb_alpha) : nullptr;

			//Write the roughness, metallic, ambient occlusion and displacement data to the file.
			constexpr byte defaultRoughness{ 255 };
			constexpr byte defaultMetallic{ 0 };
			constexpr byte defaultAmbientOcclusion{ 255 };
			constexpr byte defaultDisplacement{ 0 };

			for (uint8 i = 0; i < parameters._MipmapLevels; ++i)
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