#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Vector2.h>

class VegetationCullingComponent final
{

public:

	//Container for all the grid cell center locations.
	DynamicArray<Vector2> gridCellCenterLocations;

	//The squared cutoff distance to compare with.
	float cutoffDistance;

};