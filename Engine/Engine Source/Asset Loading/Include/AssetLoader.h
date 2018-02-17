#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
#include <TerrainMaterialData.h>

class AssetLoader final
{

public:

	/*
	*	Given a file path, load terrain material data.
	*/
	static void LoadTerrainMaterialData(const char *RESTRICT filePath, TerrainMaterialData &terrainMaterialData) NOEXCEPT;

};