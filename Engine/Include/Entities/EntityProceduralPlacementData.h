#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Entities.
#include <Entities/EntitiesCore.h>
#include <Entities/EntityProceduralPlacementGridPoint.h>

class EntityProceduralPlacementData final
{

public:

	//The function.
	EntityProceduralPlacementFunction _Function;

	//The size of each grid point.
	float _GridSize;

	//The grid points.
	StaticArray<EntityProceduralPlacementGridPoint, 27> _GridPoints;

};