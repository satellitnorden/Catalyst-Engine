//Header file.
#include <Resources/ResourceLoader.h>

//Multithreading.
#include <Multithreading/Task.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/ResourceLoaderUtilities.h>
#include <Resources/ResourcesCore.h>
#include <Resources/SoundBankData.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/VegetationMaterialData.h>
#include <Resources/WaterMaterialData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Static variable definitions.
Map<HashString, EnvironmentMaterial> ResourceLoader::environmentMaterials;
Map<HashString, PhysicalMaterial> ResourceLoader::physicalMaterials;
Map<HashString, PhysicalModel> ResourceLoader::physicalModels;
Map<HashString, SoundBank> ResourceLoader::soundBanks;
Map<HashString, TerrainMaterial> ResourceLoader::terrainMaterials;
Map<HashString, VegetationMaterial> ResourceLoader::vegetationMaterials;
Map<HashString, WaterMaterial> ResourceLoader::waterMaterials;

/*
*	Given a file path, load a resource collection.
*/
void ResourceLoader::LoadResourceCollection(char *RESTRICT filePath, Semaphore *const RESTRICT semaphore) NOEXCEPT
{
	//Launch the task.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		ResourceLoader::LoadResourceCollectionInternal(static_cast<const char *const RESTRICT>(arguments));
	}, filePath, semaphore));
}

/*
*	Loads a resource collection, internal implementation.
*/
void ResourceLoader::LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT
{
	//Load the file.
	BinaryFile<IOMode::In> file{ filePath };

	//Read the number of resources.
	uint64 numberOfResources;
	file.Read(&numberOfResources, sizeof(uint64));

	//For each resource, load it.
	for (uint64 i = 0; i < numberOfResources; ++i)
	{
		//Read the resource type.
		ResourceType resourceType;
		file.Read(&resourceType, sizeof(ResourceType));

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

			case ResourceType::SoundBank:
			{
				LoadSoundBank(file);

				break;
			}

			case ResourceType::TerrainMaterial:
			{
				LoadTerrainMaterial(file);

				break;
			}

			case ResourceType::VegetationMaterial:
			{
				LoadVegetationMaterial(file);

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
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the resolution of the diffuse data.
	file.Read(&environmentMaterialData.diffuseResolution, sizeof(uint32));

	//Calculate the diffuse data size.
	const uint64 diffuseDataSize{ environmentMaterialData.diffuseResolution * environmentMaterialData.diffuseResolution * 4 * sizeof(float) * 6 };

	//Upsize the diffuse data accordingly.
	environmentMaterialData.diffuseData.UpsizeFast(environmentMaterialData.diffuseResolution * environmentMaterialData.diffuseResolution * 4 * 6);

	//Read the diffuse data.
	file.Read(environmentMaterialData.diffuseData.Data(), diffuseDataSize);

	//Read the resolution of the diffuse irradiance data.
	file.Read(&environmentMaterialData.diffuseIrradianceResolution, sizeof(uint32));

	//Calculate the diffuse irradiance data size.
	const uint64 diffuseIrradianceDataSize{ environmentMaterialData.diffuseIrradianceResolution * environmentMaterialData.diffuseIrradianceResolution * 4 * sizeof(float) * 6 };

	//Upsize the diffuse irradiance data accordingly.
	environmentMaterialData.diffuseIrradianceData.UpsizeFast(environmentMaterialData.diffuseIrradianceResolution * environmentMaterialData.diffuseIrradianceResolution * 4 * 6);

	//Read the diffuse iraddiance data.
	file.Read(environmentMaterialData.diffuseIrradianceData.Data(), diffuseIrradianceDataSize);

	//Create the environment material via the rendering system.
	RenderingSystem::Instance->CreateEnvironmentMaterial(environmentMaterialData, environmentMaterials[resourceID]);
}

/*
*	Given a file, load a physical material.
*/
void ResourceLoader::LoadPhysicalMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the physical material data in the physical material data structure.
	PhysicalMaterialData physicalMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&physicalMaterialData.mipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&physicalMaterialData.width, sizeof(uint32));

	//Read the height.
	file.Read(&physicalMaterialData.height, sizeof(uint32));

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
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

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

	//Create the physical model via the rendering system.
	RenderingSystem::Instance->CreatePhysicalModel(physicalModelData, physicalModels[resourceID]);
}

/*
*	Given a file, load a sound bank.
*/
void ResourceLoader::LoadSoundBank(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the sound bank data in the sound bank data structure.
	SoundBankData soundBankData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the sound bank size.
	file.Read(&soundBankData.size, sizeof(uint64));

	//Resize the sound bank data accordingly.
	soundBankData.data.UpsizeFast(soundBankData.size);

	//Read the sound bank data.
	file.Read(soundBankData.data.Data(), soundBankData.size);

	//Load the sound bank via the sound system.
	SoundSystem::Instance->LoadSoundBank(soundBankData, soundBanks[resourceID]);
}

/*
*	Given a file, load a terrain material.
*/
void ResourceLoader::LoadTerrainMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the terrain material data in the terrain material data structure.
	TerrainMaterialData terrainMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

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

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, terrainMaterials[resourceID]);
}

/*
*	Given a file, load a vegetation material.
*/
void ResourceLoader::LoadVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the vegetation material data in the vegetation material data structure.
	VegetationMaterialData vegetationMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the mask mipmap levels.
	file.Read(&vegetationMaterialData.maskMipmapLevels, sizeof(uint8));

	//Read the remaining mipmap levels.
	file.Read(&vegetationMaterialData.remainingMipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&vegetationMaterialData.width, sizeof(uint32));

	//Read the height.
	file.Read(&vegetationMaterialData.height, sizeof(uint32));

	//Read the mask data.
	vegetationMaterialData.maskData.UpsizeSlow(vegetationMaterialData.maskMipmapLevels);

	const uint64 textureSize{ vegetationMaterialData.width * vegetationMaterialData.height * 4 };

	for (uint8 i = 0; i < vegetationMaterialData.maskMipmapLevels; ++i)
	{
		vegetationMaterialData.maskData[i].Reserve(textureSize >> i);

		file.Read(vegetationMaterialData.maskData[i].Data(), textureSize >> i);
	}

	//Read the albedo.
	vegetationMaterialData.albedoData.UpsizeSlow(vegetationMaterialData.remainingMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData.remainingMipmapLevels; ++i)
	{
		vegetationMaterialData.albedoData[i].Reserve(textureSize >> i);

		file.Read(vegetationMaterialData.albedoData[i].Data(), textureSize >> i);
	}

	//Read the normal map.
	vegetationMaterialData.normalMapData.UpsizeSlow(vegetationMaterialData.remainingMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData.remainingMipmapLevels; ++i)
	{
		vegetationMaterialData.normalMapData[i].Reserve(textureSize >> i);

		file.Read(vegetationMaterialData.normalMapData[i].Data(), textureSize >> i);
	}

	//Create the vegetation material via the rendering system.
	RenderingSystem::Instance->CreateVegetationMaterial(vegetationMaterialData, vegetationMaterials[resourceID]);
}

/*
*	Given a file, load a water material.
*/
void ResourceLoader::LoadWaterMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the water material data in the water material data structure.
	WaterMaterialData waterMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&waterMaterialData.mipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&waterMaterialData.width, sizeof(uint32));

	//Read the height.
	file.Read(&waterMaterialData.height, sizeof(uint32));

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