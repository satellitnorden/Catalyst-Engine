//Header file.
#include <AssetLoader.h>

//Asset loading.
#include <AssetLoaderUtilities.h>
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

	//Load the first layer.
	AssetLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.firstLayerAlbedoData, terrainMaterialData.firstLayerNormalMapData, terrainMaterialData.firstLayerMaterialPropertiesData);

	//Load the second layer.
	AssetLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.secondLayerAlbedoData, terrainMaterialData.secondLayerNormalMapData, terrainMaterialData.secondLayerMaterialPropertiesData);

	//Load the third layer.
	AssetLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.thirdLayerAlbedoData, terrainMaterialData.thirdLayerNormalMapData, terrainMaterialData.thirdLayerMaterialPropertiesData);

	//Load the fourth layer.
	AssetLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.fourthLayerAlbedoData, terrainMaterialData.fourthLayerNormalMapData, terrainMaterialData.fourthLayerMaterialPropertiesData);

	//Load the fifth layer.
	AssetLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.fifthLayerAlbedoData, terrainMaterialData.fifthLayerNormalMapData, terrainMaterialData.fifthLayerMaterialPropertiesData);

	//Close the file.
	file.Close();

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}