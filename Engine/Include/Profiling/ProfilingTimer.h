#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Systems.
#include <Systems/ProfilingSystem.h>

class ProfilingTimer final
{

public:

	/*
	*	Default constructor, prohitibed - must be constructed with the proper arguments.
	*/
	FORCE_INLINE ProfilingTimer() NOEXCEPT = delete;

	/*
	*	Constructor taking the name of the profiling entry.
	*/
	FORCE_INLINE ProfilingTimer(const char *const RESTRICT profiling_entry_name) NOEXCEPT
		:
		_ProfilingEntryName(profiling_entry_name)
	{
		//Store the start time.
		_StartTime = std::chrono::high_resolution_clock::now();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ProfilingTimer() NOEXCEPT
	{
		//Calculate the duration.
		const float duration{ static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _StartTime).count()) / 1'000'000.0f };

		//Add the profiling entry.
		ProfilingSystem::AddProfilingEntry(_ProfilingEntryName, duration);
	}

private:

	//The profiling entry name.
	DynamicString _ProfilingEntryName;

	//The start time.
	std::chrono::time_point<std::chrono::steady_clock> _StartTime;

};