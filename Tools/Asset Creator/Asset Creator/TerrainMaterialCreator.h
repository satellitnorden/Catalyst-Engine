#pragma once

//Asset creator.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Utilities.h"

//STL.
#include <fstream>
#include <string>

#define NUMBER_OF_TERRAIN_LAYERS 3

namespace TerrainMaterialCreator
{

	void CreateTerrainMaterial(const int argumentCount, char *arguments[]) noexcept
	{
		//What should the material be called?
		std::string terrainMaterialName{ arguments[2] + std::string(".ctm") };

		//Open the file to be written to.
		std::ofstream terrainMaterialFile{ terrainMaterialName.c_str(), std::ios::out | std::ios::binary };

		//Determine how many mipmap levels that should be generated.
		const unsigned int numberOfMipmapLevels{ static_cast<unsigned int>(*arguments[3] - '0') };

		//Write the number of mipmap levels to the file, to be read into a uint8.
		terrainMaterialFile.write(reinterpret_cast<const char*>(&numberOfMipmapLevels), sizeof(char));

		for (unsigned int i = 0; i < NUMBER_OF_TERRAIN_LAYERS; ++i)
		{
			//Load the layer albedo.
			int width, height, numberOfChannels;
			unsigned char *data{ stbi_load(arguments[4 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

			//Write the width and height of the layer into the file, to be read into uint32's.
			terrainMaterialFile.write(reinterpret_cast<const char*>(&width), sizeof(int));
			terrainMaterialFile.write(reinterpret_cast<const char*>(&height), sizeof(int));

			//Write the layer albedo to the file, to be read into byte's.
			unsigned long int textureSize{ static_cast<unsigned int>(width * height * 4) };

			for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
			{
				terrainMaterialFile.write(reinterpret_cast<char*>(data), textureSize >> i);
			}

			//Free the layer albedo data.
			stbi_image_free(data);

			//Load the layer normal map.
			data = stbi_load(arguments[5 + (6 * i)], &width, &height, &numberOfChannels, STBI_rgb_alpha);

			//Write the layer albedo to the file, to be read into byte's.
			for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
			{
				terrainMaterialFile.write(reinterpret_cast<char*>(data), textureSize >> i);
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
				for (unsigned int j = 0; j < (textureSize >> i); ++j)
				{
					terrainMaterialFile.write(roughnessData ? reinterpret_cast<const char*>(&roughnessData[j]) : reinterpret_cast<const char*>(&defaultRoughness), sizeof(char));
					terrainMaterialFile.write(metallicData ? reinterpret_cast<const char*>(&metallicData[j]) : reinterpret_cast<const char*>(&defaultMetallic), sizeof(char));
					terrainMaterialFile.write(ambientOcclusionData ? reinterpret_cast<const char*>(&ambientOcclusionData[j]) : reinterpret_cast<const char*>(&defaultAmbientOcclusion), sizeof(char));
					terrainMaterialFile.write(displacementData ? reinterpret_cast<const char*>(&displacementData[j]) : reinterpret_cast<const char*>(&defaultDisplacement), sizeof(char));
				}
			}

			//Free the roughness, metallic, ambient occlusion and displacement data.
			stbi_image_free(roughnessData);
			stbi_image_free(metallicData);
			stbi_image_free(ambientOcclusionData);
			stbi_image_free(displacementData);
		}

		//Close the file
		terrainMaterialFile.close();
	}

}