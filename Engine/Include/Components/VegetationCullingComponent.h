#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>

class VegetationCullingComponent final
{

public:

	//Container for all the grid cell center locations.
	DynamicArray<Vector2> _GridCellCenterLocations;

	//The squared cutoff distance to compare with.
	float _CutoffDistance;

};