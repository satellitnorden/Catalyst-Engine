#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Entities.
#include <Entities/Core/EntityCore.h>
#include <Entities/Types/Entity.h>

//Math.
#include <Math/Geometry/GridPoint2.h>
#include <Math/Geometry/GridPoint3.h>

class EntityPlacementData final
{

public:

	//Entity grid point union.
	union EntityGridPoint
	{
		GridPoint2 _GridPoint2;
		GridPoint3 _GridPoint3;
	};

	//The type.
	EntityPlacementType _Type;

	//The function.
	EntityPlacementFunction _Function;

	//The grid size.
	uint8 _GridSize;

	//The grid cell size.
	float _GridCellSize;

	//Container for all grid points.
	DynamicArray<EntityGridPoint> _GridPoints;

	//Container for all entities.
	DynamicArray<DynamicArray<Entity *RESTRICT>> _Entities;

};