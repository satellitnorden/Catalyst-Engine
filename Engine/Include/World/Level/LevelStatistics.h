#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox3D.h>

class LevelStatistics final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox3D _AxisAlignedBoundingBox;

	//The radius of the level.
	float32 _Radius;

};