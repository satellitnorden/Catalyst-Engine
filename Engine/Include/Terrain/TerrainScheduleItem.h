#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint2.h>

class TerrainScheduleItem final
{

public:

	//The index.
	uint8 _Index;

	//The grid point.
	GridPoint2 _GridPoint;

	//The patch size multiplier.
	float _PatchSizeMultiplier;

};