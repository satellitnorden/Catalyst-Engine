//Header file.
#include <Resources/Loading/ResourceLoader.h>

//Multithreading.
#include <Multithreading/Task.h>

//Resources.
#include <Resources/Data/EnvironmentMaterialData.h>
#include <Resources/Data/GrassVegetationMaterialData.h>
#include <Resources/Data/GrassVegetationModelData.h>
#include <Resources/Data/OceanMaterialData.h>
#include <Resources/Data/ParticleMaterialData.h>
#include <Resources/Data/PhysicalMaterialData.h>
#include <Resources/Data/PhysicalModelData.h>
#include <Resources/Data/SoundBankData.h>
#include <Resources/Data/TreeVegetationMaterialData.h>
#include <Resources/Data/TreeVegetationModelData.h>
#include <Resources/Loading/ResourceLoaderUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Static variable definitions.
Map<HashString, EnvironmentMaterial> ResourceLoader::_EnvironmentMaterials;
Map<HashString, GrassVegetationMaterial> ResourceLoader::_GrassVegetationMaterials;
Map<HashString, GrassVegetationModel> ResourceLoader::_GrassVegetationModels;
Map<HashString, OceanMaterial> ResourceLoader::_OceanMaterials;
Map<HashString, ParticleMaterial> ResourceLoader::_ParticleMaterials;
Map<HashString, PhysicalMaterial> ResourceLoader::_PhysicalMaterials;
Map<HashString, PhysicalModel> ResourceLoader::_PhysicalModels;
Map<HashString, TreeVegetationMaterial> ResourceLoader::_TreeVegetationMaterials;
Map<HashString, SoundBankHandle> ResourceLoader::_SoundBanks;
Map<HashString, TreeVegetationModel> ResourceLoader::_TreeVegetationModels;

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
#if defined(CATALYST_CONFIGURATION_DEBUG)
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

			case ResourceType::GrassVegetationMaterial:
			{
				LoadGrassVegetationMaterial(file);

				break;
			}

			case ResourceType::GrassVegetationModel:
			{
				LoadGrassVegetationModel(file);

				break;
			}

			case ResourceType::OceanMaterial:
			{
				LoadOceanMaterial(file);

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

			case ResourceType::SoundBank:
			{
				LoadSoundBank(file);

				break;
			}

			case ResourceType::TreeVegetationMaterial:
			{
				LoadTreeVegetationMaterial(file);

				break;
			}

			case ResourceType::TreeVegetationModel:
			{
				LoadTreeVegetationModel(file);

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
*	Given a file, load a grass vegetation material.
*/
void ResourceLoader::LoadGrassVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
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
	file.Read(data._Vertices.Data(), sizeof(VegetationVertex) * numberOfVertices);

	//Read the number of indices.
	uint64 numberOfIndices;
	file.Read(&numberOfIndices, sizeof(uint64));

	//Read the indices.
	data._Indices.UpsizeFast(numberOfIndices);
	file.Read(data._Indices.Data(), sizeof(uint32) * numberOfIndices);

	//Create the grass vegetation model via the rendering system.
	RenderingSystem::Instance->CreateGrassVegetationModel(data, _GrassVegetationModels[resourceID]);
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

	//Load the models.
	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Read the extent of the physical model.
		file.Read(&physicalModelData._Extents[i], sizeof(float));

		//Read the number of vertices.
		uint64 numberOfVertices;
		file.Read(&numberOfVertices, sizeof(uint64));

		//Read the vertices.
		physicalModelData._Vertices[i].UpsizeFast(numberOfVertices);
		file.Read(physicalModelData._Vertices[i].Data(), sizeof(Vertex) * numberOfVertices);

		//Read the number of indices.
		uint64 numberOfIndices;
		file.Read(&numberOfIndices, sizeof(uint64));

		//Read the indices.
		physicalModelData._Indices[i].UpsizeFast(numberOfIndices);
		file.Read(physicalModelData._Indices[i].Data(), sizeof(uint32) * numberOfIndices);
	}

	//Create the physical model via the rendering system.
	RenderingSystem::Instance->CreatePhysicalModel(physicalModelData, _PhysicalModels[resourceID]);
}

/*
*	Given a file, load a sound bank.
*/
void ResourceLoader::LoadSoundBank(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the sound bank data in the sound bank data structure.
	SoundBankData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the size of the sound bank.
	uint64 soundBankSize{ 0 };
	file.Read(&soundBankSize, sizeof(uint64));

	//Reserve the required amount of memory.
	data._Data.UpsizeFast(soundBankSize);

	//Read the sound bank memory.
	file.Read(data._Data.Data(), soundBankSize);

	//Create the sound bank via the sound system.
	SoundSystem::Instance->CreateSoundBank(data, &_SoundBanks[resourceID]);
}

/*
*	Given a file, load a tree vegetation material.
*/
void ResourceLoader::LoadTreeVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the tree vegetation material data in the tree vegetation material data structure.
	TreeVegetationMaterialData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	//Read the number of mask mipmap levels for the crown mask.
	file.Read(&data._CrownMaskMipmapLevels, sizeof(uint8));

	//Read the crown mask width.
	file.Read(&data._CrownMaskWidth, sizeof(uint32));

	//Read the crown mask height.
	file.Read(&data._CrownMaskHeight, sizeof(uint32));

	//Read the crown mask data.
	data._CrownMaskData.UpsizeSlow(data._CrownMaskMipmapLevels);

	for (uint8 i = 0; i < data._CrownMaskMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._CrownMaskWidth >> i) * (data._CrownMaskHeight >> i) * 4 };

		data._CrownMaskData[i].Reserve(textureSize);

		file.Read(data._CrownMaskData[i].Data(), textureSize);
	}

	//Read the number of mipmap levels for the crown.
	file.Read(&data._CrownMipmapLevels, sizeof(uint8));

	//Read the crown width.
	file.Read(&data._CrownWidth, sizeof(uint32));

	//Read the crown height.
	file.Read(&data._CrownHeight, sizeof(uint32));

	//Read the crown albedo data.
	data._CrownAlbedoData.UpsizeSlow(data._CrownMipmapLevels);

	for (uint8 i = 0; i < data._CrownMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._CrownWidth >> i) * (data._CrownHeight >> i) * 4 };

		data._CrownAlbedoData[i].Reserve(textureSize);

		file.Read(data._CrownAlbedoData[i].Data(), textureSize);
	}

	//Read the crown normal map data.
	data._CrownNormalMapData.UpsizeSlow(data._CrownMipmapLevels);

	for (uint8 i = 0; i < data._CrownMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._CrownWidth >> i) * (data._CrownHeight >> i) * 4 };

		data._CrownNormalMapData[i].Reserve(textureSize);

		file.Read(data._CrownNormalMapData[i].Data(), textureSize);
	}

	//Read the number of mask mipmap levels for the impostor mask.
	file.Read(&data._ImpostorMaskMipmapLevels, sizeof(uint8));

	//Read the impostor mask width.
	file.Read(&data._ImpostorMaskWidth, sizeof(uint32));

	//Read the impostor mask height.
	file.Read(&data._ImpostorMaskHeight, sizeof(uint32));

	//Read the impostor mask data.
	data._ImpostorMaskData.UpsizeSlow(data._ImpostorMaskMipmapLevels);

	for (uint8 i = 0; i < data._ImpostorMaskMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._ImpostorMaskWidth >> i) * (data._ImpostorMaskHeight >> i) * 4 };

		data._ImpostorMaskData[i].Reserve(textureSize);

		file.Read(data._ImpostorMaskData[i].Data(), textureSize);
	}

	//Read the number of mipmap levels for the impostor.
	file.Read(&data._ImpostorMipmapLevels, sizeof(uint8));

	//Read the impostor width.
	file.Read(&data._ImpostorWidth, sizeof(uint32));

	//Read the impostor height.
	file.Read(&data._ImpostorHeight, sizeof(uint32));

	//Read the impostor albedo data.
	data._ImpostorAlbedoData.UpsizeSlow(data._ImpostorMipmapLevels);

	for (uint8 i = 0; i < data._ImpostorMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._ImpostorWidth >> i) * (data._ImpostorHeight >> i) * 4 };

		data._ImpostorAlbedoData[i].Reserve(textureSize);

		file.Read(data._ImpostorAlbedoData[i].Data(), textureSize);
	}

	//Read the number of mipmap levels for the trunk.
	file.Read(&data._TrunkMipmapLevels, sizeof(uint8));

	//Read the trunk width.
	file.Read(&data._TrunkWidth, sizeof(uint32));

	//Read the height.
	file.Read(&data._TrunkHeight, sizeof(uint32));

	//Read the trunk albedo.
	data._TrunkAlbedoData.UpsizeSlow(data._TrunkMipmapLevels);

	for (uint8 i = 0; i < data._TrunkMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._TrunkWidth >> i) * (data._TrunkHeight >> i) * 4 };

		data._TrunkAlbedoData[i].Reserve(textureSize);

		file.Read(data._TrunkAlbedoData[i].Data(), textureSize);
	}

	//Read the trunk normal map.
	data._TrunkNormalMapData.UpsizeSlow(data._TrunkMipmapLevels);

	for (uint8 i = 0; i < data._TrunkMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._TrunkWidth >> i) * (data._TrunkHeight >> i) * 4 };

		data._TrunkNormalMapData[i].Reserve(textureSize);

		file.Read(data._TrunkNormalMapData[i].Data(), textureSize);
	}

	//Read the trunk material properties.
	data._TrunkMaterialPropertiesData.UpsizeSlow(data._TrunkMipmapLevels);

	for (uint8 i = 0; i < data._TrunkMipmapLevels; ++i)
	{
		const uint64 textureSize{ (data._TrunkWidth >> i) * (data._TrunkHeight >> i) * 4 };

		data._TrunkMaterialPropertiesData[i].Reserve(textureSize);

		file.Read(data._TrunkMaterialPropertiesData[i].Data(), textureSize);
	}

	//Create the tree vegetation material via the rendering system.
	RenderingSystem::Instance->CreateTreeVegetationMaterial(data, _TreeVegetationMaterials[resourceID]);
}

/*
*	Given a file, load a tree vegetation model.
*/
void ResourceLoader::LoadTreeVegetationModel(BinaryFile<IOMode::In> &file) NOEXCEPT
{
	//Store the grass vegetation model data in the tree vegetation model data structure.
	TreeVegetationModelData data;

	//Read the resource ID.
	HashString resourceID;
	file.Read(&resourceID, sizeof(HashString));

	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Read the extent for the crown of the tree vegetation model.
		file.Read(&data._CrownExtents[i], sizeof(float));

		//Read the number of vertices for the crown.
		uint64 numberOfVertices;
		file.Read(&numberOfVertices, sizeof(uint64));

		//Read the vertices for the crown.
		data._CrownVertices[i].UpsizeFast(numberOfVertices);
		file.Read(data._CrownVertices[i].Data(), sizeof(VegetationVertex) * numberOfVertices);

		//Read the number of indices for the crown.
		uint64 numberOfIndices;
		file.Read(&numberOfIndices, sizeof(uint64));

		//Read the indices for the crown.
		data._CrownIndices[i].UpsizeFast(numberOfIndices);
		file.Read(data._CrownIndices[i].Data(), sizeof(uint32) * numberOfIndices);
	}

	for (uint8 i{ 0 }; i < UNDERLYING(LevelOfDetail::NumberOfLevelOfDetails); ++i)
	{
		//Read the extent for the trunk of the tree vegetation model.
		file.Read(&data._TrunkExtents[i], sizeof(float));

		//Read the number of vertices for the trunk.
		uint64 numberOfVertices;
		file.Read(&numberOfVertices, sizeof(uint64));

		//Read the vertices for the trunk.
		data._TrunkVertices[i].UpsizeFast(numberOfVertices);
		file.Read(data._TrunkVertices[i].Data(), sizeof(VegetationVertex) * numberOfVertices);

		//Read the number of indices for the trunk.
		uint64 numberOfIndices;
		file.Read(&numberOfIndices, sizeof(uint64));

		//Read the indices for the trunk.
		data._TrunkIndices[i].UpsizeFast(numberOfIndices);
		file.Read(data._TrunkIndices[i].Data(), sizeof(uint32) * numberOfIndices);
	}

	//Create the tree vegetation model via the rendering system.
	RenderingSystem::Instance->CreateTreeVegetationModel(data, _TreeVegetationModels[resourceID]);
}