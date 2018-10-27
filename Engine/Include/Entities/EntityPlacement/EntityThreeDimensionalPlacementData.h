#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Entities.
#include <Entities/EntityPlacement/EntityPlacementCore.h>
#include <Entities/EntityPlacement/EntityThreeDimensionalGridPoint.h>

class EntityThreeDimensionalPlacementData final
{

public:

	//The function.
	EntityPlacementFunction _Function;

	//The size of each grid point.
	float _GridSize;

	//The grid points.
	StaticArray<EntityThreeDimensionalGridPoint, 27> _GridPoints;

};