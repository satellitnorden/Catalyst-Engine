#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Time.h>

template <typename TYPE>
class DeltaTimer final
{

public:

	/*
	*	Resets this delta timer.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		_CurrentTimePoint.Reset();
	}

	/*
	*	Updates the current time and returns the delta time.
	*/
	FORCE_INLINE NO_DISCARD TYPE Update() NOEXCEPT
	{
		const TYPE delta_time{ static_cast<TYPE>(_CurrentTimePoint.GetSecondsSince()) };
		_CurrentTimePoint.Reset();

		return delta_time;
	}

private:

	//The current time point.
	TimePoint _CurrentTimePoint;

};