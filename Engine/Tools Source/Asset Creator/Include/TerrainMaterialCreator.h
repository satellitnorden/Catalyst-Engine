#pragma once

//Engine core.
#include <EngineCore.h>

//Asset creator.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#define NUMBER_OF_TERRAIN_LAYERS 3

namespace TerrainMaterialCreator
{

	void CreateTerrainMaterial(const int32 argumentCount, char *RESTRICT arguments[]) noexcept
	{
		//What should the material be called?
		DynamicString terrainMaterialName{ arguments[2] };
		terrainMaterialName += ".ctm";

		//Open the file to be written to.
		BinaryFile<IOMode::Out> terrainMaterialFile{ terrainMaterialName.CString() };

		//Determine how many mipmap levels that should be generated.
		unsigned int numberOfMipmapLevels{ static_cast<unsigned int>(*arguments[3] - '0') };

		//Write the number of mipmap levels to the file, to be read into a uint8.
		terrainMaterialFile.Write(&numberOfMipmapLevels, sizeof(char));

		for (unsigned int i = 0; i < NUMBER_OF_TERRAIN_LAYERS; ++i)
		{
			//Load the layer albedo.
			int width, height, numberOfChannels;
			unsigned char *data{ stbi_load(arguments[4 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			//Write the width and height of the layer into the file, to be read into uint32's.
			terrainMaterialFile.Write(&width, sizeof(int));
			terrainMaterialFile.Write(&height, sizeof(int));

			//Write the layer albedo to the file, to be read into byte's.
			unsigned long int textureSize{ static_cast<unsigned int>(width * height * 4) };

			for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
			{
				//If this is the base mipmap level, just copy the thing directly into memory.
				if (i == 0)
				{
					terrainMaterialFile.Write(data, textureSize >> i);
				}

				//Else, the image data should be resized.
				else
				{
					unsigned char *downsampledData = static_cast<unsigned char*>(malloc(textureSize >> i));
					stbir_resize_uint8(data, width, height, 0, downsampledData, width >> i, height >> i, 0, 4);

					terrainMaterialFile.Write(downsampledData, textureSize >> i);

					free(downsampledData);
				}
			}

			//Free the layer albedo data.
			stbi_image_free(data);

			//Load the layer normal map.
			data = stbi_load(arguments[5 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the layer albedo to the file, to be read into byte's.
			for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
			{
				//If this is the base mipmap level, just copy the thing directly into memory.
				if (i == 0)
				{
					terrainMaterialFile.Write(data, textureSize >> i);
				}

				//Else, the image data should be resized.
				else
				{
					unsigned char *downsampledData = static_cast<unsigned char*>(malloc(textureSize >> i));
					stbir_resize_uint8(data, width, height, 0, downsampledData, width >> i, height >> i, 0, 4);

					terrainMaterialFile.Write(downsampledData, textureSize >> i);

					free(downsampledData);
				}
			}

			//Free the layer normal map data.
			stbi_image_free(data);

			//Load the roughness, metallic, ambient occlusion and displacement data.
			unsigned char *roughnessData = stbi_load(arguments[6 + (6 * i)], &width, &height, &numberOfChannels, 0);
			unsigned char *metallicData = stbi_load(arguments[7 + (6 * i)], &width, &height, &numberOfChannels, 0);
			unsigned char *ambientOcclusionData = stbi_load(arguments[8 + (6 * i)], &width, &height, &numberOfChannels, 0);
			unsigned char *displacementData = stbi_load(arguments[9 + (6 * i)], &width, &height, &numberOfChannels, 0);

			//Write the roughness, metallic, ambient occlusion and displacement data to the file.
			unsigned char defaultRoughness{ 255 };
			unsigned char defaultMetallic{ 0 };
			unsigned char defaultAmbientOcclusion{ 255 };
			unsigned char defaultDisplacement{ 0 };

			textureSize = static_cast<unsigned int>(width * height);

			for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
			{
				//If this is the base mipmap level, treat it differently.
				if (i == 0)
				{
					for (unsigned int j = 0; j < textureSize; ++j)
					{
						terrainMaterialFile.Write(roughnessData ? &roughnessData[j] : &defaultRoughness, sizeof(char));
						terrainMaterialFile.Write(metallicData ? &metallicData[j] : &defaultMetallic, sizeof(char));
						terrainMaterialFile.Write(ambientOcclusionData ? &ambientOcclusionData[j] : &defaultAmbientOcclusion, sizeof(char));
						terrainMaterialFile.Write(displacementData ? &displacementData[j] : &defaultDisplacement, sizeof(char));
					}
				}

				else
				{
					unsigned char *downsampledRoughnessData = roughnessData ? static_cast<unsigned char*>(malloc(textureSize >> i)) : nullptr;
					unsigned char *downsampledMetallicData = metallicData ? static_cast<unsigned char*>(malloc(textureSize >> i)) : nullptr;
					unsigned char *downsampledAmbientOcclusionData = ambientOcclusionData ? static_cast<unsigned char*>(malloc(textureSize >> i)) : nullptr;
					unsigned char *downsampledDisplacementData = displacementData ? static_cast<unsigned char*>(malloc(textureSize >> i)) : nullptr;

					if (roughnessData) stbir_resize_uint8(roughnessData, width, height, 0, downsampledRoughnessData, width >> i, height >> i, 0, 1);
					if (metallicData) stbir_resize_uint8(metallicData, width, height, 0, downsampledMetallicData, width >> i, height >> i, 0, 1);
					if (ambientOcclusionData) stbir_resize_uint8(ambientOcclusionData, width, height, 0, downsampledAmbientOcclusionData, width >> i, height >> i, 0, 1);
					if (displacementData) stbir_resize_uint8(displacementData, width, height, 0, downsampledDisplacementData, width >> i, height >> i, 0, 1);

					for (unsigned int j = 0; j < textureSize >> i; ++j)
					{
						terrainMaterialFile.Write(downsampledRoughnessData ? &downsampledRoughnessData[j] : &defaultRoughness, sizeof(char));
						terrainMaterialFile.Write(downsampledMetallicData ? &downsampledMetallicData[j] : &defaultMetallic, sizeof(char));
						terrainMaterialFile.Write(downsampledAmbientOcclusionData ? &downsampledAmbientOcclusionData[j] : &defaultAmbientOcclusion, sizeof(char));
						terrainMaterialFile.Write(downsampledDisplacementData ? &downsampledDisplacementData[j] : &defaultDisplacement, sizeof(char));
					}

					free(downsampledRoughnessData);
					free(downsampledMetallicData);
					free(downsampledAmbientOcclusionData);
					free(downsampledDisplacementData);
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