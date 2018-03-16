//Header file.
#include <Resources/ResourceLoader.h>

//Rendering.
#include <Rendering/Engine Layer/EnvironmentMaterial.h>
#include <Rendering/Engine Layer/PhysicalMaterial.h>
#include <Rendering/Engine Layer/PhysicalModel.h>
#include <Rendering/Engine Layer/TerrainMaterial.h>
#include <Rendering/Engine Layer/WaterMaterial.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/ResourceLoaderUtilities.h>
#include <Resources/ResourcesCore.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Static variable definitions.
Map<ResourceID, EnvironmentMaterial> ResourceLoader::environmentMaterials;
Map<ResourceID, PhysicalMaterial> ResourceLoader::physicalMaterials;
Map<ResourceID, PhysicalModel> ResourceLoader::physicalModels;
Map<ResourceID, TerrainMaterial> ResourceLoader::terrainMaterials;
Map<ResourceID, WaterMaterial> ResourceLoader::waterMaterials;

/*
*	Given a file path, load a resource collection.
*/
void ResourceLoader::LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT
{
	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the number of resources.
	uint64 numberOfResources;
	file.Read(&numberOfResources, SizeOf(uint64));

	//For each resource, load it.
	for (uint64 i = 0; i < numberOfResources; ++i)
	{
		//Read the resource type.
		ResourceType resourceType;
		file.Read(&resourceType, SizeOf(ResourceType));

		switch (resourceType)
		{
#if !defined(CATALYST_FINAL)
			default:
			{
				PRINT_TO_CONSOLE("Undefined resource type.");
				
				BREAKPOINT;
			}
#endif

			case ResourceType::EnvironmentMaterial:
			{
				LoadEnvironmentMaterial(file);

				break;
			}

			case ResourceType::PhysicalMaterial:
			{
				LoadPhysicalMaterial(file);

				break;
			}

			case ResourceType::PhysicalModel:
			{
				LoadPhysicalModel(file);

				break;
			}

			case ResourceType::TerrainMaterial:
			{
				LoadTerrainMaterial(file);

				break;
			}

			case ResourceType::WaterMaterial:
			{
				LoadWaterMaterial(file);

				break;
			}
		}
	}
}

/*
*	Given a file, load an environment material.
*/
void ResourceLoader::LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the environment material data in the environment material data structure.
	EnvironmentMaterialData environmentMaterialData;

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, SizeOf(ResourceID));

	//Read the number environment data size.
	uint64 environmentDataSize;
	file.Read(&environmentDataSize, SizeOf(uint64));

	//Resize the data accoordingly.
	environmentMaterialData.albedoData.UpsizeFast(environmentDataSize);
	environmentMaterialData.diffuseData.UpsizeFast(environmentDataSize);
	environmentMaterialData.diffuseIrradianceData.UpsizeFast(environmentDataSize);

	//Read the data.
	file.Read(environmentMaterialData.albedoData.Data(), environmentMaterialData.albedoData.Size() * SizeOf(float));
	file.Read(environmentMaterialData.diffuseData.Data(), environmentMaterialData.diffuseData.Size() * SizeOf(float));
	file.Read(environmentMaterialData.diffuseIrradianceData.Data(), environmentMaterialData.diffuseIrradianceData.Size() * SizeOf(float));

	//Create the physical material via the rendering system.
	//RenderingSystem::Instance->CreatePhysicalMaterial(environmentMaterialData, environmentMaterials[resourceID]);
}

/*
*	Given a file, load a physical material.
*/
void ResourceLoader::LoadPhysicalMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the physical material data in the physical material data structure.
	PhysicalMaterialData physicalMaterialData;

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, SizeOf(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&physicalMaterialData.mipmapLevels, SizeOf(uint8));

	//Read the width.
	file.Read(&physicalMaterialData.width, SizeOf(uint32));

	//Read the height.
	file.Read(&physicalMaterialData.height, SizeOf(uint32));

	//Read the albedo.
	physicalMaterialData.albedoData.UpsizeSlow(physicalMaterialData.mipmapLevels);

	const uint64 textureSize{ physicalMaterialData.width * physicalMaterialData.height * 4 };

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.albedoData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.albedoData[i].Data(), textureSize >> i);
	}

	//Read the normal map.
	physicalMaterialData.normalMapData.UpsizeSlow(physicalMaterialData.mipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.normalMapData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.normalMapData[i].Data(), textureSize >> i);
	}

	//Read the material properties.
	physicalMaterialData.materialPropertiesData.UpsizeSlow(physicalMaterialData.mipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData.mipmapLevels; ++i)
	{
		physicalMaterialData.materialPropertiesData[i].Reserve(textureSize >> i);

		file.Read(physicalMaterialData.materialPropertiesData[i].Data(), textureSize >> i);
	}

	//Create the physical material via the rendering system.
	RenderingSystem::Instance->CreatePhysicalMaterial(physicalMaterialData, physicalMaterials[resourceID]);
}

/*
*	Given a file, load a physical model.
*/
void ResourceLoader::LoadPhysicalModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the physical model data in the physical model data structure.
	PhysicalModelData physicalModelData;

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, SizeOf(ResourceID));

	//Read the extent of the physical model.
	file.Read(&physicalModelData.extent, SizeOf(float));

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, SizeOf(uint64));

	//Read the vertices.
	physicalModelData.vertices.UpsizeFast(numberOfVertices);
	file.Read(physicalModelData.vertices.Data(), SizeOf(PhysicalVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, SizeOf(uint64));

	//Read the indices.
	physicalModelData.indices.UpsizeFast(numberOfIndices);
	file.Read(physicalModelData.indices.Data(), SizeOf(uint32) * numberOfIndices);

	//Create the physical model via the rendering system.
	RenderingSystem::Instance->CreatePhysicalModel(physicalModelData, physicalModels[resourceID]);
}

/*
*	Given a file, load a terrain material.
*/
void ResourceLoader::LoadTerrainMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the terrain material data in the terrain material data structure.
	TerrainMaterialData terrainMaterialData;

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, SizeOf(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&terrainMaterialData.mipmapLevels, SizeOf(uint8));

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

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterials[resourceID]);
}

/*
*	Given a file, load a water material.
*/
void ResourceLoader::LoadWaterMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the water material data in the water material data structure.
	WaterMaterialData waterMaterialData;

	//Read the resource ID.
	ResourceID resourceID;
	file.Read(&resourceID, SizeOf(ResourceID));

	//Read the number of mipmap levels.
	file.Read(&waterMaterialData.mipmapLevels, SizeOf(uint8));

	//Read the width.
	file.Read(&waterMaterialData.width, SizeOf(uint32));

	//Read the height.
	file.Read(&waterMaterialData.height, SizeOf(uint32));

	//Read the normal map.
	waterMaterialData.normalMapData.UpsizeSlow(waterMaterialData.mipmapLevels);

	const uint64 textureSize{ waterMaterialData.width * waterMaterialData.height * 4 };

	for (uint8 i = 0; i < waterMaterialData.mipmapLevels; ++i)
	{
		waterMaterialData.normalMapData[i].Reserve(textureSize >> i);

		file.Read(waterMaterialData.normalMapData[i].Data(), textureSize >> i);
	}

	//Create the water material via the rendering system.
	RenderingSystem::Instance->CreateWaterMaterial(waterMaterialData, waterMaterials[resourceID]);
}