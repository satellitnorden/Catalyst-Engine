#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ModelFadeData final
{

public:

	//The start fade out distance.
	float32 _StartFadeOutDistance;

	//The end fade out distance.
	float32 _EndFadeOutDistance;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ModelFadeData() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE ModelFadeData(const float32 initial_start_fade_out_distance, const float32 initial_end_fade_out_distance) NOEXCEPT
		:
		_StartFadeOutDistance(initial_start_fade_out_distance),
		_EndFadeOutDistance(initial_end_fade_out_distance)
	{

	}

};