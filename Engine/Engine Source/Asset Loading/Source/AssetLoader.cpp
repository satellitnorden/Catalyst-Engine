//Header file.
#include <AssetLoader.h>

//Asset loading.
#include <TerrainMaterialData.h>

//STL.
#include <fstream>

//Preprocessor defines.
#define TO_BYTE(data) static_cast<char *RESTRICT>(static_cast<void *RESTRICT>(&data))

/*
*	Given a file path, load terrain material data.
*/
void AssetLoader::LoadTerrainMaterialData(const char *RESTRICT filePath, TerrainMaterialData &terrainMaterialData) NOEXCEPT
{
	//Load the file.
	std::ifstream file{ filePath, std::ios::in | std::ios::binary };

	//Read the number of mipmap levels.
	file.read(TO_BYTE(terrainMaterialData.mipmapLevels), sizeof(uint8));

	//Read the width of the first layer.
	file.read(TO_BYTE(terrainMaterialData.firstLayerWidth), sizeof(uint32));

	//Read the height of the first layer.
	file.read(TO_BYTE(terrainMaterialData.firstLayerHeight), sizeof(uint32));

	//Read the first layer albedo.
	terrainMaterialData.firstLayerAlbedoData.Resize(terrainMaterialData.mipmapLevels);

	uint64 textureSize{ terrainMaterialData.firstLayerWidth * terrainMaterialData.firstLayerHeight * 4 };

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerAlbedoData[i].Reserve(textureSize >> i);

		file.read(TO_BYTE(*terrainMaterialData.firstLayerAlbedoData[i].Data()), textureSize >> i);
	}

	//Read the first layer normal map.
	terrainMaterialData.firstLayerNormalMapData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerNormalMapData[i].Reserve(textureSize >> i);

		file.read(TO_BYTE(*terrainMaterialData.firstLayerNormalMapData[i].Data()), textureSize >> i);
	}

	//Read the first layer properties.
	terrainMaterialData.firstLayerPropertiesData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerPropertiesData[i].Reserve(textureSize >> i);

		file.read(TO_BYTE(*terrainMaterialData.firstLayerPropertiesData[i].Data()), textureSize >> i);
	}

	//Close the file.
	file.close();
}

//Preprocessor undefines.
#undef TO_BYTE