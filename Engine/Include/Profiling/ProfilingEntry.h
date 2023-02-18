#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

class ProfilingEntry final
{

public:

	/*
	*	Default constructor, prohibited - must be constructed with proper arguments..
	*/
	FORCE_INLINE ProfilingEntry() NOEXCEPT = delete;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE ProfilingEntry(const char *const RESTRICT initial_name, const float32 initial_frame_duration, const float32 initial_average_duration, const float32 inifial_maximum_duration) NOEXCEPT
		:
		_Name(initial_name),
		_FrameDuration(initial_frame_duration),
		_AverageDuration(initial_average_duration),
		_MaximumDuration(inifial_maximum_duration)
	{

	}

	//The name.
	const char *RESTRICT _Name;

	//The frame duration.
	float32 _FrameDuration;

	//The average duration.
	float32 _AverageDuration;

	//The maximum duration.
	float32 _MaximumDuration;

};