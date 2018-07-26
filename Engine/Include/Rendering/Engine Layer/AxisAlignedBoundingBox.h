#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Math.
#include <Math/Vector3.h>

class AxisAlignedBoundingBox final
{

public:

	//The minimum corner.
	Vector3 minimum;

	//The maximum corner.
	Vector3 maximum;

};