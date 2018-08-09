#pragma once

//Core.
#include <Core/EngineCore.h>

class VegetationProperties final
{

public:

	//The cutoff distance.
	float cutoffDistance;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationProperties(const float initialCutoffDistance) NOEXCEPT
		:
		cutoffDistance(initialCutoffDistance)
	{

	}

};