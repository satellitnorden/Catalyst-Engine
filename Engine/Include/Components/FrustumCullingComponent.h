#pragma once

//Core.
#include <Core/General/EngineCore.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

class FrustumCullingComponent final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

};