#pragma once

//Engine core.
#include <EngineCore.h>

//Graphics.
#include <AxisAlignedBoundingBox.h>

class FrustumCullingComponent
{

public:

	//Whether or not the entity is in the view frustum.
	bool isInViewFrustum{ true };

	//The axis aligned bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

};