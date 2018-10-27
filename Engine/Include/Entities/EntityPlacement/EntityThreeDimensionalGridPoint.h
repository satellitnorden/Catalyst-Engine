#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Entity.h>

//Math.
#include <Math/GridPoint3.h>

class EntityThreeDimensionalGridPoint final
{

public:

	//Denotes whether or not this grid point is valid.
	bool _Valid;

	//The grid point.
	GridPoint3 _GridPoint;

	//Container for all entities contained in this grid point.
	DynamicArray<Entity *RESTRICT> _Entities;

};