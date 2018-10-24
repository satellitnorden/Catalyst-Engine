#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/GridPoint2.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class VegetationPatchInformation final
{

public:

	//Denotes whether or not this patch is valid.
	bool _Valid;

	//The grid point.
	GridPoint2 _GridPoint;

	//The axis aligned bounding box.
	AxisAlignedBoundingBox _AxisAlignedBoundingBox;

};