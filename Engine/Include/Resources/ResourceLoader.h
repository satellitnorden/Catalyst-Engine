#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/Map.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/OceanMaterial.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Resources.
#include <Resources/ResourcesCore.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>
#include <Vegetation/TreeVegetationModel.h>

class ResourceLoader final
{

public:

	/*
	*	Given a file path, loads a resource collection.
	*/
	static void LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a resource ID, return the corresponding environment material.
	*/
	static const EnvironmentMaterial& GetEnvironmentMaterial(const HashString resourceID) { return _EnvironmentMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding grass vegetation material.
	*/
	static const GrassVegetationMaterial& GetGrassVegetationMaterial(const HashString resourceID) { return _GrassVegetationMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding grass vegetation model.
	*/
	static const GrassVegetationModel& GetGrassVegetationModel(const HashString resourceID) { return _GrassVegetationModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding ocean material.
	*/
	static const OceanMaterial& GetOceanMaterial(const HashString resourceID) { return _OceanMaterials[resourceID]; }

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
	*	Given a resource ID, return the corresponding tree vegetation model.
	*/
	static const TreeVegetationModel& GetTreeVegetationModel(const HashString resourceID) { return _TreeVegetationModels[resourceID]; }

private:

	//Container for all environment materials.
	static Map<HashString, EnvironmentMaterial> _EnvironmentMaterials;

	//Container for all grass vegetation materials.
	static Map<HashString, GrassVegetationMaterial> _GrassVegetationMaterials;

	//Container for all grass vegetation models.
	static Map<HashString, GrassVegetationModel> _GrassVegetationModels;

	//Container for all ocean materials.
	static Map<HashString, OceanMaterial> _OceanMaterials;

	//Container for all particle materials.
	static Map<HashString, ParticleMaterial> _ParticleMaterials;

	//Container for all physical materials.
	static Map<HashString, PhysicalMaterial> _PhysicalMaterials;

	//Container for all physical models.
	static Map<HashString, PhysicalModel> _PhysicalModels;

	//Container for all tree vegetation models.
	static Map<HashString, TreeVegetationModel> _TreeVegetationModels;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a file, load an environment material.
	*/
	static void LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a grass material.
	*/
	static void LoadGrassMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a grass vegetation model.
	*/
	static void LoadGrassVegetationModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

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
	*	Given a file, load a tree vegetation model.
	*/
	static void LoadTreeVegetationModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

};