#pragma once

//Engine core.
#include <EngineCore.h>

//Forward declarations.
class TerrainMaterial;

class AssetLoader final
{

public:

	/*
	*	Given a file path, load a terrain material.
	*/
	static void LoadTerrainMaterial(const char *RESTRICT filePath, TerrainMaterial &terrainMaterial) NOEXCEPT;

};