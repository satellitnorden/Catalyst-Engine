#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class TerrainProperties final
{

public:

	//The height of the terrain.
	float _Height;

	//The resolution of each patch.
	uint32 _PatchResolution;

	//The size of each patch.
	float _PatchSize;

};