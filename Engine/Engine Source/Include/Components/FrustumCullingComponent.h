#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/AxisAlignedBoundingBox.h>

class FrustumCullingComponent final
{

public:

	//Whether or not the entity is in the view frustum.
	bool isInViewFrustum{ true };

	//The axis aligned bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

};