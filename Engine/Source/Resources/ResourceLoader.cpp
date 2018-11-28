//Header file.
#include <Resources/ResourceLoader.h>

//Multithreading.
#include <Multithreading/Task.h>

//Resources.
#include <Resources/EnvironmentMaterialData.h>
#include <Resources/GrassVegetationMaterialData.h>
#include <Resources/GrassVegetationModelData.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Resources/OceanMaterialData.h>
#endif
#include <Resources/ParticleMaterialData.h>
#include <Resources/PhysicalMaterialData.h>
#include <Resources/PhysicalModelData.h>
#include <Resources/ResourceLoaderUtilities.h>
#include <Resources/ResourcesCore.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Static variable definitions.
Map<HashString, EnvironmentMaterial> ResourceLoader::_EnvironmentMaterials;
Map<HashString, GrassVegetationMaterial> ResourceLoader::_GrassVegetationMaterials;
Map<HashString, GrassVegetationModel> ResourceLoader::_GrassVegetationModels;
#if defined(CATALYST_ENABLE_OCEAN)
Map<HashString, OceanMaterial> ResourceLoader::_OceanMaterials;
#endif
Map<HashString, ParticleMaterial> ResourceLoader::_ParticleMaterials;
Map<HashString, PhysicalMaterial> ResourceLoader::_PhysicalMaterials;
Map<HashString, PhysicalModel> ResourceLoader::_PhysicalModels;

/*
*	Given a file path, load a resource collection.
*/
void ResourceLoader::LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT
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

			case ResourceType::GrassMaterial:
			{
				LoadGrassMaterial(file);

				break;
			}

			case ResourceType::GrassVegetationModel:
			{
				LoadGrassVegetationModel(file);

				break;
			}

#if defined(CATALYST_ENABLE_OCEAN)
			case ResourceType::OceanMaterial:
			{
				LoadOceanMaterial(file);

				break;
			}
#endif

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
*	Given a file, load a grass material.
*/
void ResourceLoader::LoadGrassMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the grass vegetation material data in the grass vegetation material data structure.
	GrassVegetationMaterialData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mask mipmap levels.
	file.Read(&data._MaskMipmapLevels, sizeof(uint8));

	//Read the mask width.
	file.Read(&data._MaskWidth, sizeof(uint32));

	//Read the mask height.
	file.Read(&data._MaskHeight, sizeof(uint32));

	//Read the mask data.
	data._MaskData.UpsizeSlow(data._MaskMipmapLevels);

	for (uint8 i = 0; i < data._MaskMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._MaskWidth >> i) * (data._MaskHeight >> i) * 4 };

		data._MaskData[i].Reserve(textureSize);

		file.Read(data._MaskData[i].Data(), textureSize);
	}

	//Read the number of mipmap levels.
	file.Read(&data._MipmapLevels, sizeof(uint8));

	//Read the width.
	file.Read(&data._Width, sizeof(uint32));

	//Read the height.
	file.Read(&data._Height, sizeof(uint32));

	//Read the albedo data.
	data._AlbedoData.UpsizeSlow(data._MipmapLevels);

	for (uint8 i = 0; i < data._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._Width >> i) * (data._Height >> i) * 4 };

		data._AlbedoData[i].Reserve(textureSize);

		file.Read(data._AlbedoData[i].Data(), textureSize);
	}

	//Read the normal map data.
	data._NormalMapData.UpsizeSlow(data._MipmapLevels);

	for (uint8 i = 0; i < data._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._Width >> i) * (data._Height >> i) * 4 };

		data._NormalMapData[i].Reserve(textureSize);

		file.Read(data._NormalMapData[i].Data(), textureSize);
	}

	//Create the grass material via the rendering system.
	RenderingSystem::Instance->CreateGrassVegetationMaterial(data, _GrassVegetationMaterials[resourceID]);
}

/*
*	Given a file, load a grass vegetation model.
*/
void ResourceLoader::LoadGrassVegetationModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the grass vegetation model data in the grass vegetation model data structure.
	GrassVegetationModelData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the extent of the grass vegetation model.
	file.Read(&data._Extent, sizeof(float));

	//Read the number of vertices.
	uint64 numberOfVertices;
	file.Read(&numberOfVertices, sizeof(uint64));

	//Read the vertices.
	data._Vertices.UpsizeFast(numberOfVertices);
	file.Read(data._Vertices.Data(), sizeof(GrassVegetationVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	data._Indices.UpsizeFast(numberOfIndices);
	file.Read(data._Indices.Data(), sizeof(uint32) * numberOfIndices);

	//Create the grass vegetation model via the rendering system.
	RenderingSystem::Instance->CreateGrassVegetationModel(data, _GrassVegetationModels[resourceID]);
}

#if defined(CATALYST_ENABLE_OCEAN)
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

	//Read the normal data.
	oceanMaterialData._NormalData.UpsizeSlow(oceanMaterialData._MipmapLevels);

	for (uint8 i = 0; i < oceanMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (oceanMaterialData._Width >> i) * (oceanMaterialData._Height >> i) * 4 };

		oceanMaterialData._NormalData[i].Reserve(textureSize);

		file.Read(oceanMaterialData._NormalData[i].Data(), textureSize);
	}

	//Read the foam data.
	oceanMaterialData._FoamData.UpsizeSlow(oceanMaterialData._MipmapLevels);

	for (uint8 i = 0; i < oceanMaterialData._MipmapLevels; ++i)
	{
		const uint64 textureSize{ (oceanMaterialData._Width >> i) * (oceanMaterialData._Height >> i) * 4 };

		oceanMaterialData._FoamData[i].Reserve(textureSize);

		file.Read(oceanMaterialData._FoamData[i].Data(), textureSize);
	}

	//Create the ocean material via the rendering system.
	RenderingSystem::Instance->CreateOceanMaterial(oceanMaterialData, _OceanMaterials[resourceID]);
}
#endif

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