#pragma once

//Asset creator.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//STL.
#include <fstream>
#include <string>

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
		terrainMaterialFile.write(reinterpret_cast<const char*>(&numberOfMipmapLevels), 1);

		//Load the first layer albedo.
		int width, height, numberOfChannels;
		unsigned char *data{ stbi_load(arguments[4], &width, &height, &numberOfChannels, STBI_rgb_alpha) };

		//Write the width and height of the first layer into the file, to be read into uint32's.
		unsigned int unsignedWidth{ static_cast<unsigned int>(width) };
		unsigned int unsignedHeight{ static_cast<unsigned int>(height) };

		terrainMaterialFile.write(reinterpret_cast<const char*>(&unsignedWidth), 4);
		terrainMaterialFile.write(reinterpret_cast<const char*>(&unsignedHeight), 4);

		//Write the first layer albedo to the file, to be read into byte's.
		unsigned int textureSize{ static_cast<unsigned int>(width * height * 4) };

		for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
		{
			terrainMaterialFile.write(reinterpret_cast<char*>(data), textureSize >> i);
		}

		//Free the first layer albedo data.
		stbi_image_free(data);

		//Load the first layer normal map.
		data = stbi_load(arguments[5], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the first layer albedo to the file, to be read into byte's.
		for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
		{
			terrainMaterialFile.write(reinterpret_cast<char*>(data), textureSize >> i);
		}

		//Free the first layer normal map data.
		stbi_image_free(data);

		//Load the roughness, metallic, ambient occlusion and displacement data.
		unsigned char *roughnessData = stbi_load(arguments[6], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		unsigned char *metallicData = stbi_load(arguments[7], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		unsigned char *ambientOcclusionData = stbi_load(arguments[8], &width, &height, &numberOfChannels, STBI_rgb_alpha);
		unsigned char *displacementData = stbi_load(arguments[9], &width, &height, &numberOfChannels, STBI_rgb_alpha);

		//Write the roughness, metallic, ambient occlusion and displacement data to the file.
		char defaultRoughness{ 1 };
		char defaultMetallic{ 0 };
		char defaultAmbientOcclusion{ 1 };
		char defaultDisplacement{ 1 };

		for (unsigned int i = 0; i < numberOfMipmapLevels; ++i)
		{
			for (unsigned int j = 0; j < (textureSize >> j); ++j)
			{
				terrainMaterialFile.write(roughnessData ? reinterpret_cast<char*>(&roughnessData[j]) : &defaultRoughness, 1);
				terrainMaterialFile.write(metallicData ? reinterpret_cast<char*>(&metallicData[j]) : &defaultMetallic, 1);
				terrainMaterialFile.write(ambientOcclusionData ? reinterpret_cast<char*>(&ambientOcclusionData[j]) : &defaultAmbientOcclusion, 1);
				terrainMaterialFile.write(displacementData ? reinterpret_cast<char*>(&displacementData[j]) : &defaultDisplacement, 1);
			}
		}

		//Free the roughness, metallic, ambient occlusion and displacement data.
		stbi_image_free(roughnessData);
		stbi_image_free(metallicData);
		stbi_image_free(ambientOcclusionData);
		stbi_image_free(displacementData);

		//Close the file
		terrainMaterialFile.close();
	}

}