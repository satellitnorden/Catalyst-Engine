//Header file.
#include <Core/General/Time.h>

//STL.
#include <chrono>

#define IMPLEMENTATION() (*_Implementation.Get< std::chrono::time_point<std::chrono::steady_clock>>())

/*
*	Default constructor.
*/
TimePoint::TimePoint() NOEXCEPT
{
	//Reset.
	Reset();
}

/*
*	Reset this time point.
*/
void TimePoint::Reset() NOEXCEPT
{
	//Set the time point.
	IMPLEMENTATION() = std::chrono::steady_clock::now();
}

/*
*	Returns the amount of seconds since this time point.
*/
NO_DISCARD float64 TimePoint::GetSecondsSince() const NOEXCEPT
{
	return static_cast<float64>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - IMPLEMENTATION()).count()) / 1'000'000'000.0;
}

/*
*	Adds seconds.
*/
void TimePoint::AddSeconds(const float64 value) NOEXCEPT
{
	IMPLEMENTATION() += std::chrono::nanoseconds(static_cast<uint64>(value * 1'000'000'000));
}