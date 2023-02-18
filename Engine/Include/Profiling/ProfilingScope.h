#if !defined(CATALYST_CONFIGURATION_FINAL)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/ProfilingSystem.h>

/*
*	Profiling scope static data class definition.
*/
class ProfilingScopeStaticData final
{

public:

	//The average duration.
	uint64 _AverageDuration{ 0 };

	//The maximum duration.
	uint64 _MaximumDuration{ 0 };

};

/*
*	Profiling scope class definition.
*/
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
	FORCE_INLINE ProfilingScope(const char *const RESTRICT profiling_entry_name, ProfilingScopeStaticData *const RESTRICT data) NOEXCEPT
		:
		_ProfilingEntryName(profiling_entry_name),
		_Data(data)
	{
		//Store the start time.
		_StartTime = std::chrono::high_resolution_clock::now();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ProfilingScope() NOEXCEPT
	{
		//Calculate the frame duration.
		const uint64 frame_duration{ static_cast<uint64>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _StartTime).count()) };

		//Update the average duration.
		_Data->_AverageDuration = static_cast<uint64>(CatalystBaseMath::LinearlyInterpolate(static_cast<float64>(frame_duration), static_cast<float64>(_Data->_AverageDuration), 0.9));

		//Update the maximum duration.
		_Data->_MaximumDuration = CatalystBaseMath::Maximum<uint64>(_Data->_MaximumDuration, frame_duration);

		//Add the profiling entry.
		ProfilingSystem::Instance->AddProfilingEntry(_ProfilingEntryName, static_cast<float32>(frame_duration) / 1'000'000.0f, static_cast<float32>(_Data->_AverageDuration) / 1'000'000.0f, static_cast<float32>(_Data->_MaximumDuration) / 1'000'000.0f);
	}

private:

	//The profiling entry name.
	const char *RESTRICT _ProfilingEntryName;

	//The data.
	ProfilingScopeStaticData *const RESTRICT _Data;

	//The start time.
	std::chrono::time_point<std::chrono::steady_clock> _StartTime;

};
#endif