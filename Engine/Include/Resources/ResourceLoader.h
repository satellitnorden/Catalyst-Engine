#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Map.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>
#include <Rendering/Engine/TerrainMaterial.h>
#if defined(CATALYST_ENABLE_OCEAN)
#include <Rendering/Engine/OceanMaterial.h>
#endif

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
	static const EnvironmentMaterial& GetEnvironmentMaterial(const HashString resourceID) { return _EnvironmentMaterials[resourceID]; }

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Given a resource ID, return the corresponding ocean material.
	*/
	static const OceanMaterial& GetOceanMaterial(const HashString resourceID) { return _OceanMaterials[resourceID]; }
#endif

	/*
	*	Given a resource ID, return the corresponding particle material.
	*/
	static const ParticleMaterial& GetParticleMaterial(const HashString resourceID) { return _ParticleMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical material.
	*/
	static const PhysicalMaterial& GetPhysicalMaterial(const HashString resourceID) { return _PhysicalMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical model.
	*/
	static const PhysicalModel& GetPhysicalModel(const HashString resourceID) { return _PhysicalModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding terrain material.
	*/
	static const TerrainMaterial& GetTerrainMaterial(const HashString resourceID) { return _TerrainMaterials[resourceID]; }

private:

	//Container for all environment materials.
	static Map<HashString, EnvironmentMaterial> _EnvironmentMaterials;

#if defined(CATALYST_ENABLE_OCEAN)
	//Container for all ocean materials.
	static Map<HashString, OceanMaterial> _OceanMaterials;
#endif

	//Container for all particle materials.
	static Map<HashString, ParticleMaterial> _ParticleMaterials;

	//Container for all physical materials.
	static Map<HashString, PhysicalMaterial> _PhysicalMaterials;

	//Container for all physical models.
	static Map<HashString, PhysicalModel> _PhysicalModels;

	//Container for all terrain materials.
	static Map<HashString, TerrainMaterial> _TerrainMaterials;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a file, load an environment material.
	*/
	static void LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

#if defined(CATALYST_ENABLE_OCEAN)
	/*
	*	Given a file, load an ocean material.
	*/
	static void LoadOceanMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;
#endif

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

};