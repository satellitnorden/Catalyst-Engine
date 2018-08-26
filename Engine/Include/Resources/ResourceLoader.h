#pragma once

//Core.
#include <Core/EngineCore.h>
#include <Core/HashString.h>
#include <Core/Containers/Map.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/TerrainMaterial.h>
#include <Rendering/Engine/VegetationMaterial.h>
#include <Rendering/Engine/OceanMaterial.h>

//Resources.
#include <Resources/ResourcesCore.h>

//Forward declarations.
class Semaphore;

class ResourceLoader final
{

public:

	/*
	*	Given a file path, loads a resource collection.
	*/
	static void LoadResourceCollection(char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a resource ID, return the corresponding environment material.
	*/
	static const EnvironmentMaterial& GetEnvironmentMaterial(const HashString resourceID) { return environmentMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding ocean material.
	*/
	static const OceanMaterial& GetOceanMaterial(const HashString resourceID) { return oceanMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding particle material.
	*/
	static const ParticleMaterial& GetParticleMaterial(const HashString resourceID) { return particleMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical material.
	*/
	static const PhysicalMaterial& GetPhysicalMaterial(const HashString resourceID) { return physicalMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical model.
	*/
	static const PhysicalModel& GetPhysicalModel(const HashString resourceID) { return physicalModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding terrain material.
	*/
	static const TerrainMaterial& GetTerrainMaterial(const HashString resourceID) { return terrainMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding vegetation material.
	*/
	static const VegetationMaterial& GetVegetationMaterial(const HashString resourceID) { return vegetationMaterials[resourceID]; }

private:

	//Container for all environment materials.
	static Map<HashString, EnvironmentMaterial> environmentMaterials;

	//Container for all ocean materials.
	static Map<HashString, OceanMaterial> oceanMaterials;

	//Container for all particle materials.
	static Map<HashString, ParticleMaterial> particleMaterials;

	//Container for all physical materials.
	static Map<HashString, PhysicalMaterial> physicalMaterials;

	//Container for all physical models.
	static Map<HashString, PhysicalModel> physicalModels;

	//Container for all terrain materials.
	static Map<HashString, TerrainMaterial> terrainMaterials;

	//Container for all vegetation materials.
	static Map<HashString, VegetationMaterial> vegetationMaterials;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a file, load an environment material.
	*/
	static void LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load an ocean material.
	*/
	static void LoadOceanMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a particle material.
	*/
	static void LoadParticleMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a physical material.
	*/
	static void LoadPhysicalMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a physical model.
	*/
	static void LoadPhysicalModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a terrain material.
	*/
	static void LoadTerrainMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a vegetation material.
	*/
	static void LoadVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

};