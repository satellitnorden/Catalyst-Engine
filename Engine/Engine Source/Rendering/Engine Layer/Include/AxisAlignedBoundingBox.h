#pragma once

//Engine core.
#include <EngineCore.h>

//Math.
#include <Vector3.h>

class AxisAlignedBoundingBox final
{

public:

	//The minimum corner.
	Vector3 minimum;

	//The maximum corner.
	Vector3 maximum;

};