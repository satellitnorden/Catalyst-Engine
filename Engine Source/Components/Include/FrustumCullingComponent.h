#pragma once

//Engine core.
#include <EngineCore.h>

class FrustumCullingComponent
{

public:

	//Whether or not the entity is in the view frustum.
	bool isInViewFrustum{ true };

	//The model extent.
	float modelExtent{ 0.0f };

};