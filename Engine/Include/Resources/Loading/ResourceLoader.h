#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/BinaryFile.h>
#include <Core/General/HashString.h>

//Rendering.
#include <Rendering/Engine/Texture2D.h>
#include <Rendering/Engine/EnvironmentMaterial.h>
#include <Rendering/Engine/Model.h>
#include <Rendering/Engine/OceanMaterial.h>
#include <Rendering/Engine/ParticleMaterial.h>
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Resources.
#include <Resources/Core/ResourcesCore.h>

//Sound.
#include <Sound/Native/SoundCore.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>
#include <Vegetation/TreeVegetationMaterial.h>
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
	*	Given a resource ID, return the corresponding sound bank.
	*/
	static const SoundBankHandle& GetSoundBank(const HashString resourceID) { return _SoundBanks[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding tree vegetation material.
	*/
	static const TreeVegetationMaterial& GetTreeVegetationMaterial(const HashString resourceID) { return _TreeVegetationMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding tree vegetation model.
	*/
	static const TreeVegetationModel& GetTreeVegetationModel(const HashString resourceID) { return _TreeVegetationModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding model.
	*/
	static const Model& GetModel(const HashString resourceID) { return _Models[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding texture 2D
	*/
	static const Texture2D<Vector4<byte>>& GetTexture2D(const HashString resourceID) { return _Texture2Ds[resourceID]; }

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

	//Container for all sound banks.
	static Map<HashString, SoundBankHandle> _SoundBanks;

	//Container for all tree vegetation materials.
	static Map<HashString, TreeVegetationMaterial> _TreeVegetationMaterials;

	//Container for all tree vegetation models.
	static Map<HashString, TreeVegetationModel> _TreeVegetationModels;

	//Container for all models.
	static Map<HashString, Model> _Models;

	//Container for all texture 2Ds.
	static Map<HashString, Texture2D<Vector4<byte>>> _Texture2Ds;

	/*
	*	Loads a resource collection, internal implementation.
	*/
	static void LoadResourceCollectionInternal(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a file, load an environment material.
	*/
	static void LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a grass vegetation material.
	*/
	static void LoadGrassVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

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
	*	Given a file, load a sound bank.
	*/
	static void LoadSoundBank(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a tree vegetation material.
	*/
	static void LoadTreeVegetationMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a tree vegetation model.
	*/
	static void LoadTreeVegetationModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a model.
	*/
	static void LoadModel(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a texture cube.
	*/
	static void LoadTextureCube(BinaryFile<IOMode::In> &file) NOEXCEPT;

	/*
	*	Given a file, load a texture 2D.
	*/
	static void LoadTexture2D(BinaryFile<IOMode::In> &file) NOEXCEPT;

};