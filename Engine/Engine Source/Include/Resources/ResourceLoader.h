#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Resources.
#include <Resources/ResourcesCore.h>

//Forward declarations.
class EnvironmentMaterial;
class PhysicalMaterial;
class PhysicalModel;
class TerrainMaterial;
class WaterMaterial;

class ResourceLoader final
{

public:

	/*
	*	Given a file path, load a resource collection.
	*/
	static void LoadResourceCollection(const char *RESTRICT filePath) NOEXCEPT;

	/*
	*	Given a resource ID, return the corresponding environment material.
	*/
	//static const EnvironmentMaterial& GetEnvironmentMaterial(const ResourceID resourceID) { return environmentMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical material.
	*/
	static const PhysicalMaterial& GetPhysicalMaterial(const ResourceID resourceID) { return physicalMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding physical model.
	*/
	static const PhysicalModel& GetPhysicalModel(const ResourceID resourceID) { return physicalModels[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding terrain material.
	*/
	static const TerrainMaterial& GetTerrainMaterial(const ResourceID resourceID) { return terrainMaterials[resourceID]; }

	/*
	*	Given a resource ID, return the corresponding water material.
	*/
	static const WaterMaterial& GetWaterMaterial(const ResourceID resourceID) { return waterMaterials[resourceID]; }

private:

	//Container for all envionment materials.
	static Map<ResourceID, EnvironmentMaterial> environmentMaterials;

	//Container for all physical materials.
	static Map<ResourceID, PhysicalMaterial> physicalMaterials;

	//Container for all physical models.
	static Map<ResourceID, PhysicalModel> physicalModels;

	//Container for all terrain materials.
	static Map<ResourceID, TerrainMaterial> terrainMaterials;

	//Container for all water materials.
	static Map<ResourceID, WaterMaterial> waterMaterials;

	/*
	*	Given a file, load an environment material.
	*/
	static void LoadEnvironmentMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

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
	*	Given a file, load a water material.
	*/
	static void LoadWaterMaterial(BinaryFile<IOMode::In> &file) NOEXCEPT;

};