#pragma once

//Engine core.
#include <EngineCore.h>

class FrustumCullingComponent
{

public:

	//Whether or not the entity is in the view frustum.
	bool isInViewFrustum;

	//The model extent.
	float modelExtent;

};