#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class FrustumCullingComponent final
{

public:

	//The model space axis aligned bounding box.
	AxisAlignedBoundingBox _ModelSpaceAxisAlignedBoundingBox;

	//The world space axis aligned bounding box.
	AxisAlignedBoundingBox _WorldSpaceAxisAlignedBoundingBox;

};