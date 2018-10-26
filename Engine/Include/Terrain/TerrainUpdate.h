#pragma once

//Core.
#include <Core/Core/CatalystCore.h>


//Terrain.
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>

class TerrainUpdate final
{

public:

	//The index.
	int8 _Index{ -1 };

	//The patch information.
	TerrainPatchInformation _PatchInformation;

	//The patch render information.
	TerrainPatchRenderInformation _PatchRenderInformation;

};