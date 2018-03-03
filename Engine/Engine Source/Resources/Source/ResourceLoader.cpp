//Header file.
#include <ResourceLoader.h>

//Rendering.
#include <TerrainMaterial.h>

//Resources.
#include <PhysicalMaterialData.h>
#include <PhysicalModelData.h>
#include <ResourceLoaderUtilities.h>
#include <ResourcesCore.h>
#include <TerrainMaterialData.h>
#include <WaterMaterialData.h>

//Systems.
#include <RenderingSystem.h>

/*
*	Given a file path, load a physical material.
*/
void ResourceLoader::LoadPhysicalMaterial(const char *RESTRICT filePath, PhysicalMaterial &physicalMaterial) NOEXCEPT
{
	//Store the physical material data in the physical material data structure.
	PhysicalMaterialData physicalMaterialData;

	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the resource type.
	ResourceType resourceType;
	file.Read(&resourceType, sizeof(ResourceType));

#if !defined(CATALYST_FINAL)
	if (resourceType != ResourceType::PhysicalMaterial)
	{
		BREAKPOINT;
	}
#endif

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, sizeof(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&physicalMaterialData.mipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&physicalMaterialData.width, sizeof(uint32));

	//Read the height.
	file.Read(&physicalMaterialData.height, sizeof(uint32));

	//Read the albedo.
	physicalMaterialData.albedoData.Resize(physicalMaterialData.mipmapLevels);

	const uint64 textureSize{ physicalMaterialData.width * physicalMaterialData.height * 4 };

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.albedoData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.albedoData[i].Data(), textureSize >> i);
	}

	//Read the normal map.
	physicalMaterialData.normalMapData.Resize(physicalMaterialData.mipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.normalMapData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.normalMapData[i].Data(), textureSize >> i);
	}

	//Read the material properties.
	physicalMaterialData.materialPropertiesData.Resize(physicalMaterialData.mipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.materialPropertiesData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.materialPropertiesData[i].Data(), textureSize >> i);
	}

	//Close the file.
	file.Close();

	//Create the physical material via the rendering system.
	RenderingSystem::Instance->CreatePhysicalMaterial(physicalMaterialData, physicalMaterial);
}

/*
*	Given a file path, load a physical model.
*/
void ResourceLoader::LoadPhysicalModel(const char *RESTRICT filePath, PhysicalModel &physicalModel) NOEXCEPT
{
	//Store the physical model data in the physical model data structure.
	PhysicalModelData physicalModelData;

	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the resource type.
	ResourceType resourceType;
	file.Read(&resourceType, sizeof(ResourceType));

#if !defined(CATALYST_FINAL)
	if (resourceType != ResourceType::PhysicalModel)
	{
		BREAKPOINT;
	}
#endif

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, sizeof(ResourceID));

	//Read the extent of the physical model.
	file.Read(&physicalModelData.extent, sizeof(float));

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, sizeof(uint64));

	//Read the vertices.
	physicalModelData.vertices.UpsizeFast(numberOfVertices);
	file.Read(physicalModelData.vertices.Data(), sizeof(PhysicalVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	physicalModelData.indices.UpsizeFast(numberOfIndices);
	file.Read(physicalModelData.indices.Data(), sizeof(uint32) * numberOfIndices);

	//Close the file.
	file.Close();

	//Create the physical model via the rendering system.
	RenderingSystem::Instance->CreatePhysicalModel(physicalModelData, physicalModel);
}

/*
*	Given a file path, load a terrain material.
*/
void ResourceLoader::LoadTerrainMaterial(const char *RESTRICT filePath, TerrainMaterial &terrainMaterial) NOEXCEPT
{
	//Store the terrain material data in the terrain material data structure.
	TerrainMaterialData terrainMaterialData;

	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the resource type.
	ResourceType resourceType;
	file.Read(&resourceType, sizeof(ResourceType));

#if !defined(CATALYST_FINAL)
	if (resourceType != ResourceType::TerrainMaterial)
	{
		BREAKPOINT;
	}
#endif

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, sizeof(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&terrainMaterialData.mipmapLevels, sizeof(uint8));

	//Load the first layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.firstLayerWidth, terrainMaterialData.firstLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.firstLayerAlbedoData, terrainMaterialData.firstLayerNormalMapData, terrainMaterialData.firstLayerMaterialPropertiesData);

	//Load the second layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.secondLayerWidth, terrainMaterialData.secondLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.secondLayerAlbedoData, terrainMaterialData.secondLayerNormalMapData, terrainMaterialData.secondLayerMaterialPropertiesData);

	//Load the third layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.thirdLayerWidth, terrainMaterialData.thirdLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.thirdLayerAlbedoData, terrainMaterialData.thirdLayerNormalMapData, terrainMaterialData.thirdLayerMaterialPropertiesData);

	//Load the fourth layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.fourthLayerWidth, terrainMaterialData.fourthLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.fourthLayerAlbedoData, terrainMaterialData.fourthLayerNormalMapData, terrainMaterialData.fourthLayerMaterialPropertiesData);

	//Load the fifth layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData.fifthLayerWidth, terrainMaterialData.fifthLayerHeight, terrainMaterialData.mipmapLevels, terrainMaterialData.fifthLayerAlbedoData, terrainMaterialData.fifthLayerNormalMapData, terrainMaterialData.fifthLayerMaterialPropertiesData);

	//Close the file.
	file.Close();

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterial);
}

/*
*	Given a file path, load a water material.
*/
void ResourceLoader::LoadWaterMaterial(const char *RESTRICT filePath, WaterMaterial &waterMaterial) NOEXCEPT
{
	//Store the water material data in the water material data structure.
	WaterMaterialData waterMaterialData;

	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the resource type.
	ResourceType resourceType;
	file.Read(&resourceType, sizeof(ResourceType));

#if !defined(CATALYST_FINAL)
	if (resourceType != ResourceType::WaterMaterial)
	{
		BREAKPOINT;
	}
#endif

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, sizeof(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&waterMaterialData.mipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&waterMaterialData.width, sizeof(uint32));

	//Read the height.
	file.Read(&waterMaterialData.height, sizeof(uint32));

	//Read the normal map.
	waterMaterialData.normalMapData.Resize(waterMaterialData.mipmapLevels);

	const uint64 textureSize{ waterMaterialData.width * waterMaterialData.height * 4 };

	for (uint8 i = 0; i < waterMaterialData.mipmapLevels; ++i)
	{
		waterMaterialData.normalMapData[i].Reserve(textureSize >> i);

		file.Read(waterMaterialData.normalMapData[i].Data(), textureSize >> i);
	}

	//Close the file.
	file.Close();

	//Create the water material via the rendering system.
	RenderingSystem::Instance->CreateWaterMaterial(waterMaterialData, waterMaterial);
}