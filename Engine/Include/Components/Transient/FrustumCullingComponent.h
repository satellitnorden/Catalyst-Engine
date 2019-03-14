#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>

class FrustumCullingComponent final
{

public:

	//The model space axis aligned bounding box.
	AxisAlignedBoundingBox _ModelSpaceAxisAlignedBoundingBox;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

};