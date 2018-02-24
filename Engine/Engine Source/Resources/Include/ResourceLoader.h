#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class PhysicalModel;
class TerrainMaterial;

class ResourceLoader final
{

public:

	/*
	*	Given a file path, load a physical model.
	*/
	static void LoadPhysicalModel(const char *RESTRICT filePath, PhysicalModel &physicalModel) NOEXCEPT;

	/*
	*	Given a file path, load a terrain material.
	*/
	static void LoadTerrainMaterial(const char *RESTRICT filePath, TerrainMaterial &terrainMaterial) NOEXCEPT;

};