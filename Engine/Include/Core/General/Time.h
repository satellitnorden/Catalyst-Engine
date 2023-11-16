#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Time point class definition.
*/
class TimePoint final
{

public:

	/*
	*	Returns the amount of seconds since this time point.
	*/
	FORCE_INLINE NO_DISCARD float64 GetSecondsSince() const NOEXCEPT
	{
		return static_cast<float64>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _TimePoint).count()) / 1'000'000'000.0;
	}

private:

	//The underlying timepoint.
	std::chrono::time_point<std::chrono::steady_clock> _TimePoint;

};

/*
*	Returns the current timepoint.
*/
FORCE_INLINE NO_DISCARD TimePoint GetCurrentTimePoint() NOEXCEPT
{
	TimePoint time_point;

	//The time point class is just a thin wrapper around the std::chrono object, so we can cast it directly. (:
	std::chrono::time_point<std::chrono::steady_clock>* const RESTRICT chrono_time_point{ (std::chrono::time_point<std::chrono::steady_clock> *const RESTRICT)(&time_point) };

	*chrono_time_point = std::chrono::steady_clock::now();

	return time_point;
}