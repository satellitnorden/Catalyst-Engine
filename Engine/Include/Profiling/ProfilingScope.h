#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/ProfilingSystem.h>

class ProfilingScope final
{

public:

	/*
	*	Default constructor, prohitibed - must be constructed with the proper arguments.
	*/
	FORCE_INLINE ProfilingScope() NOEXCEPT = delete;

	/*
	*	Constructor taking the name of the profiling entry.
	*/
	FORCE_INLINE ProfilingScope(const char *const RESTRICT profiling_entry_name) NOEXCEPT
		:
		_ProfilingEntryName(profiling_entry_name)
	{
		//Store the start time.
		_StartTime = std::chrono::high_resolution_clock::now();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ProfilingScope() NOEXCEPT
	{
		//Calculate the duration.
		const float32 duration{ static_cast<float32>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _StartTime).count()) / 1'000'000.0f };

		//Add the profiling entry.
		ProfilingSystem::Instance->AddProfilingEntry(_ProfilingEntryName, duration);
	}

private:

	//The profiling entry name.
	const char *RESTRICT _ProfilingEntryName;

	//The start time.
	std::chrono::time_point<std::chrono::steady_clock> _StartTime;

};
#endif