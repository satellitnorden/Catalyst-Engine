#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

class VegetationProperties final
{

public:

	//The cutoff distance.
	float _CutoffDistance;

	/*
	*	Constructor taking all values as arguments.
	*/
	VegetationProperties(const float initialCutoffDistance) NOEXCEPT
		:
		_CutoffDistance(initialCutoffDistance)
	{

	}

};