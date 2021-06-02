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
	FORCE_INLINE ProfilingEntry(const char *const RESTRICT initial_name, const float32 initial_duration) NOEXCEPT
		:
		_Name(initial_name),
		_Duration(initial_duration)
	{

	}

	//The name.
	const char *RESTRICT _Name;

	//The duration.
	float32 _Duration;

};