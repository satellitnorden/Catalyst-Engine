//Header file.
#include <AssetLoader.h>

//Asset loading.
#include <TerrainMaterialData.h>

//Rendering.
#include <TerrainMaterial.h>

//Systems.
#include <RenderingSystem.h>

/*
*	Given a file path, load a terrain material.
*/
void AssetLoader::LoadTerrainMaterial(const char *RESTRICT filePath, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Store the terrain material in the terrain material data structure.
	TerrainMaterialData terrainMaterialData;

	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the number of mipmap levels.
	file.Read(&terrainMaterialData.mipmapLevels, sizeof(uint8));

	//Read the width of the first layer.
	file.Read(&terrainMaterialData.firstLayerWidth, sizeof(uint32));

	//Read the height of the first layer.
	file.Read(&terrainMaterialData.firstLayerHeight, sizeof(uint32));

	//Read the first layer albedo.
	terrainMaterialData.firstLayerAlbedoData.Resize(terrainMaterialData.mipmapLevels);

	uint64 textureSize{ terrainMaterialData.firstLayerWidth * terrainMaterialData.firstLayerHeight * 4 };

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerAlbedoData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.firstLayerAlbedoData[i].Data(), textureSize >> i);
	}

	//Read the first layer normal map.
	terrainMaterialData.firstLayerNormalMapData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerNormalMapData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.firstLayerNormalMapData[i].Data(), textureSize >> i);
	}

	//Read the first layer material properties.
	terrainMaterialData.firstLayerMaterialPropertiesData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.firstLayerMaterialPropertiesData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.firstLayerMaterialPropertiesData[i].Data(), textureSize >> i);
	}

	//Read the width of the second layer.
	file.Read(&terrainMaterialData.secondLayerWidth, sizeof(uint32));

	//Read the height of the second layer.
	file.Read(&terrainMaterialData.secondLayerHeight, sizeof(uint32));

	//Read the second layer albedo.
	terrainMaterialData.secondLayerAlbedoData.Resize(terrainMaterialData.mipmapLevels);

	textureSize = terrainMaterialData.secondLayerWidth * terrainMaterialData.secondLayerHeight * 4;

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.secondLayerAlbedoData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.secondLayerAlbedoData[i].Data(), textureSize >> i);
	}

	//Read the second layer normal map.
	terrainMaterialData.secondLayerNormalMapData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.secondLayerNormalMapData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.secondLayerNormalMapData[i].Data(), textureSize >> i);
	}

	//Read the second layer material properties.
	terrainMaterialData.secondLayerMaterialPropertiesData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.secondLayerMaterialPropertiesData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.secondLayerMaterialPropertiesData[i].Data(), textureSize >> i);
	}

	//Read the width of the third layer.
	file.Read(&terrainMaterialData.thirdLayerWidth, sizeof(uint32));

	//Read the height of the third layer.
	file.Read(&terrainMaterialData.thirdLayerHeight, sizeof(uint32));

	//Read the third layer albedo.
	terrainMaterialData.thirdLayerAlbedoData.Resize(terrainMaterialData.mipmapLevels);

	textureSize = terrainMaterialData.thirdLayerWidth * terrainMaterialData.thirdLayerHeight * 4;

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.thirdLayerAlbedoData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.thirdLayerAlbedoData[i].Data(), textureSize >> i);
	}

	//Read the third layer normal map.
	terrainMaterialData.thirdLayerNormalMapData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.thirdLayerNormalMapData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.thirdLayerNormalMapData[i].Data(), textureSize >> i);
	}

	//Read the third layer material properties.
	terrainMaterialData.thirdLayerMaterialPropertiesData.Resize(terrainMaterialData.mipmapLevels);

	for (uint8 i = 0; i < terrainMaterialData.mipmapLevels; ++i)
	{
		terrainMaterialData.thirdLayerMaterialPropertiesData[i].Reserve(textureSize >> i);

		file.Read(terrainMaterialData.thirdLayerMaterialPropertiesData[i].Data(), textureSize >> i);
	}

	//Close the file.
	file.Close();

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}