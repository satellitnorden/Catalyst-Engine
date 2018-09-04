//Header file.
#include <Resources/ResourceLoader.h>

//Multithreading.
#include <Multithreading/Task.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/OceanMaterialData.h>
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/ResourceLoaderUtilities.h>
#include <Resources/ResourcesCore.h>
#include <Resources/TerrainMaterialData.h>
#include <Resources/VegetationMaterialData.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Static variable definitions.
Map<HashString, EnvironmentMaterial> ResourceLoader::_EnvironmentMaterials;
Map<HashString, OceanMaterial> ResourceLoader::_OceanMaterials;
Map<HashString, ParticleMaterial> ResourceLoader::_ParticleMaterials;
Map<HashString, PhysicalMaterial> ResourceLoader::_PhysicalMaterials;
Map<HashString, PhysicalModel> ResourceLoader::_PhysicalModels;
Map<HashString, TerrainMaterial> ResourceLoader::_TerrainMaterials;
Map<HashString, VegetationMaterial> ResourceLoader::_VegetationMaterials;

/*
*	Given a file path, load a resource collection.
*/
void ResourceLoader::LoadResourceCollection(char *RESTRICT filePath) NOEXCEPT
{
	//Load the resource collection.
	LoadResourceCollectionInternal(filePath);
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
				ASSERT(false, "Undefined resource type.");
			}
#endif

			case ResourceType::EnvironmentMaterial:
			{
				LoadEnvironmentMaterial(file);

				break;
			}

			case ResourceType::ParticleMaterial:
			{
				LoadParticleMaterial(file);

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

			case ResourceType::VegetationMaterial:
			{
				LoadVegetationMaterial(file);

				break;
			}

			case ResourceType::OceanMaterial:
			{
				LoadOceanMaterial(file);

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
	file.Read(&environmentMaterialData._DiffuseResolution, sizeof(uint32));

	//Calculate the diffuse data size.
	const uint64 diffuseDataSize{ environmentMaterialData._DiffuseResolution * environmentMaterialData._DiffuseResolution * 4 * sizeof(float) * 6 };

	//Upsize the diffuse data accordingly.
	environmentMaterialData._DiffuseData.UpsizeFast(environmentMaterialData._DiffuseResolution * environmentMaterialData._DiffuseResolution * 4 * 6);

	//Read the diffuse data.
	file.Read(environmentMaterialData._DiffuseData.Data(), diffuseDataSize);

	//Read the resolution of the diffuse irradiance data.
	file.Read(&environmentMaterialData._DiffuseIrradianceResolution, sizeof(uint32));

	//Calculate the diffuse irradiance data size.
	const uint64 diffuseIrradianceDataSize{ environmentMaterialData._DiffuseIrradianceResolution * environmentMaterialData._DiffuseIrradianceResolution * 4 * sizeof(float) * 6 };

	//Upsize the diffuse irradiance data accordingly.
	environmentMaterialData._DiffuseIrradianceData.UpsizeFast(environmentMaterialData._DiffuseIrradianceResolution * environmentMaterialData._DiffuseIrradianceResolution * 4 * 6);

	//Read the diffuse iraddiance data.
	file.Read(environmentMaterialData._DiffuseIrradianceData.Data(), diffuseIrradianceDataSize);

	//Create the environment material via the rendering system.
	RenderingSystem::Instance->CreateEnvironmentMaterial(environmentMaterialData, _EnvironmentMaterials[resourceID]);
}

/*
*	Given a file, load an ocean material.
*/
void ResourceLoader::LoadOceanMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the ocean material data in the ocean material data structure.
	OceanMaterialData oceanMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&oceanMaterialData._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&oceanMaterialData._Width, sizeof(uint32));

	//Read the height.
	file.Read(&oceanMaterialData._Height, sizeof(uint32));

	//Read the normal map.
	oceanMaterialData._NormalMapData.UpsizeSlow(oceanMaterialData._MipmapLevels);

	for (uint8 i = 0; i < oceanMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (oceanMaterialData._Width >> i) * (oceanMaterialData._Height >> i) * 4 };

		oceanMaterialData._NormalMapData[i].Reserve(textureSize);

		file.Read(oceanMaterialData._NormalMapData[i].Data(), textureSize);
	}

	//Create the ocean material via the rendering system.
	RenderingSystem::Instance->CreateOceanMaterial(oceanMaterialData, _OceanMaterials[resourceID]);
}

/*
*	Given a file, load a particle material.
*/
void ResourceLoader::LoadParticleMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the particle material data in the particle material data structure.
	ParticleMaterialData particleMaterialData;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mipmap levels.
	file.Read(&particleMaterialData._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&particleMaterialData._Width, sizeof(uint32));

	//Read the height.
	file.Read(&particleMaterialData._Height, sizeof(uint32));

	//Read the albedo.
	particleMaterialData._AlbedoData.UpsizeSlow(particleMaterialData._MipmapLevels);

	for (uint8 i = 0; i < particleMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (particleMaterialData._Width >> i) * (particleMaterialData._Height >> i) * 4 };

		particleMaterialData._AlbedoData[i].Reserve(textureSize);

		file.Read(particleMaterialData._AlbedoData[i].Data(), textureSize);
	}

	//Create the particle material via the rendering system.
	RenderingSystem::Instance->CreateParticleMaterial(particleMaterialData, _ParticleMaterials[resourceID]);
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
	file.Read(&physicalMaterialData._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&physicalMaterialData._Width, sizeof(uint32));

	//Read the height.
	file.Read(&physicalMaterialData._Height, sizeof(uint32));

	//Read the albedo.
	physicalMaterialData._AlbedoData.UpsizeSlow(physicalMaterialData._MipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (physicalMaterialData._Width >> i) * (physicalMaterialData._Height >> i) * 4 };

		physicalMaterialData._AlbedoData[i].Reserve(textureSize);

		file.Read(physicalMaterialData._AlbedoData[i].Data(), textureSize);
	}

	//Read the normal map.
	physicalMaterialData._NormalMapData.UpsizeSlow(physicalMaterialData._MipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (physicalMaterialData._Width >> i) * (physicalMaterialData._Height >> i) * 4 };

		physicalMaterialData._NormalMapData[i].Reserve(textureSize);

		file.Read(physicalMaterialData._NormalMapData[i].Data(), textureSize);
	}

	//Read the material properties.
	physicalMaterialData._MaterialPropertiesData.UpsizeSlow(physicalMaterialData._MipmapLevels);

	for (uint8 i = 0; i < physicalMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (physicalMaterialData._Width >> i) * (physicalMaterialData._Height >> i) * 4 };

		physicalMaterialData._MaterialPropertiesData[i].Reserve(textureSize);

		file.Read(physicalMaterialData._MaterialPropertiesData[i].Data(), textureSize);
	}

	//Create the physical material via the rendering system.
	RenderingSystem::Instance->CreatePhysicalMaterial(physicalMaterialData, _PhysicalMaterials[resourceID]);
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
	file.Read(&physicalModelData._Extent, sizeof(float));

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, sizeof(uint64));

	//Read the vertices.
	physicalModelData._Vertices.UpsizeFast(numberOfVertices);
	file.Read(physicalModelData._Vertices.Data(), sizeof(PhysicalVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	physicalModelData._Indices.UpsizeFast(numberOfIndices);
	file.Read(physicalModelData._Indices.Data(), sizeof(uint32) * numberOfIndices);

	//Create the physical model via the rendering system.
	RenderingSystem::Instance->CreatePhysicalModel(physicalModelData, _PhysicalModels[resourceID]);
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
	file.Read(&terrainMaterialData._MipmapLevels, sizeof(uint8));

	//Load the first layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData._FirstLayerWidth, terrainMaterialData._FirstLayerHeight, terrainMaterialData._MipmapLevels, terrainMaterialData._FirstLayerAlbedoData, terrainMaterialData._FirstLayerNormalMapData, terrainMaterialData._FirstLayerMaterialPropertiesData);

	//Load the second layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData._SecondLayerWidth, terrainMaterialData._SecondLayerHeight, terrainMaterialData._MipmapLevels, terrainMaterialData._SecondLayerAlbedoData, terrainMaterialData._SecondLayerNormalMapData, terrainMaterialData._SecondLayerMaterialPropertiesData);

	//Load the third layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData._ThirdLayerWidth, terrainMaterialData._ThirdLayerHeight, terrainMaterialData._MipmapLevels, terrainMaterialData._ThirdLayerAlbedoData, terrainMaterialData._ThirdLayerNormalMapData, terrainMaterialData._ThirdLayerMaterialPropertiesData);

	//Load the fourth layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData._FourthLayerWidth, terrainMaterialData._FourthLayerHeight, terrainMaterialData._MipmapLevels, terrainMaterialData._FourthLayerAlbedoData, terrainMaterialData._FourthLayerNormalMapData, terrainMaterialData._FourthLayerMaterialPropertiesData);

	//Load the fifth layer.
	ResourceLoaderUtilities::LoadTerrainLayerData(file, terrainMaterialData._FifthLayerWidth, terrainMaterialData._FifthLayerHeight, terrainMaterialData._MipmapLevels, terrainMaterialData._FifthLayerAlbedoData, terrainMaterialData._FifthLayerNormalMapData, terrainMaterialData._FifthLayerMaterialPropertiesData);

	//Create the terrain material via the rendering system.
	RenderingSystem::Instance->CreateTerrainMaterial(terrainMaterialData, _TerrainMaterials[resourceID]);
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
	file.Read(&vegetationMaterialData._MaskMipmapLevels, sizeof(uint8));

	//Read the remaining mipmap levels.
	file.Read(&vegetationMaterialData._RemainingMipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&vegetationMaterialData._Width, sizeof(uint32));

	//Read the height.
	file.Read(&vegetationMaterialData._Height, sizeof(uint32));

	//Read the mask data.
	vegetationMaterialData._MaskData.UpsizeSlow(vegetationMaterialData._MaskMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData._MaskMipmapLevels; ++i)
	{
		const uint64 textureSize{ (vegetationMaterialData._Width >> i) * (vegetationMaterialData._Height >> i) * 4 };

		vegetationMaterialData._MaskData[i].Reserve(textureSize);

		file.Read(vegetationMaterialData._MaskData[i].Data(), textureSize);
	}

	//Read the albedo.
	vegetationMaterialData._AlbedoData.UpsizeSlow(vegetationMaterialData._RemainingMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData._RemainingMipmapLevels; ++i)
	{
		const uint64 textureSize{ (vegetationMaterialData._Width >> i) * (vegetationMaterialData._Height >> i) * 4 };

		vegetationMaterialData._AlbedoData[i].Reserve(textureSize);

		file.Read(vegetationMaterialData._AlbedoData[i].Data(), textureSize);
	}

	//Read the normal map.
	vegetationMaterialData._NormalMapData.UpsizeSlow(vegetationMaterialData._RemainingMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData._RemainingMipmapLevels; ++i)
	{
		const uint64 textureSize{ (vegetationMaterialData._Width >> i) * (vegetationMaterialData._Height >> i) * 4 };

		vegetationMaterialData._NormalMapData[i].Reserve(textureSize);

		file.Read(vegetationMaterialData._NormalMapData[i].Data(), textureSize);
	}

	//Read the properties.
	vegetationMaterialData._PropertiesData.UpsizeSlow(vegetationMaterialData._RemainingMipmapLevels);

	for (uint8 i = 0; i < vegetationMaterialData._RemainingMipmapLevels; ++i)
	{
		const uint64 textureSize{ (vegetationMaterialData._Width >> i) * (vegetationMaterialData._Height >> i) * 4 };

		vegetationMaterialData._PropertiesData[i].Reserve(textureSize);

		file.Read(vegetationMaterialData._PropertiesData[i].Data(), textureSize);
	}

	//Create the vegetation material via the rendering system.
	RenderingSystem::Instance->CreateVegetationMaterial(vegetationMaterialData, _VegetationMaterials[resourceID]);
}