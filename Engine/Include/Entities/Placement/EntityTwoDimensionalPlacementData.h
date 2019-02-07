#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Entities.
#include <Entities/Placement/EntityPlacementCore.h>
#include <Entities/Placement/EntityTwoDimensionalGridPoint.h>

class EntityTwoDimensionalPlacementData final
{

public:

	//The function.
	EntityPlacementFunction _Function;

	//The size of each grid point.
	float _GridSize;

	//The grid points.
	StaticArray<EntityTwoDimensionalGridPoint, 9> _GridPoints;

};