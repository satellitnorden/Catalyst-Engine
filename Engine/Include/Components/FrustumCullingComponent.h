#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Rendering.
#include <Rendering/Engine Layer/AxisAlignedBoundingBox.h>

class FrustumCullingComponent final
{

public:

	//The axis aligned bounding box.
	AxisAlignedBoundingBox axisAlignedBoundingBox;

};