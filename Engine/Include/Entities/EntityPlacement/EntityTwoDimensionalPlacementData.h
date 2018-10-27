#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Entities.
#include <Entities/EntityPlacement/EntityPlacementCore.h>
#include <Entities/EntityPlacement/EntityTwoDimensionalGridPoint.h>

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