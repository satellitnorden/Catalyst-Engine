#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Any.h>

/*
*	Time point class definition.
*/
class TimePoint final
{

public:

	/*
	*	Default constructor.
	*/
	TimePoint() NOEXCEPT;

	/*
	*	Resets this time point.
	*/
	void Reset() NOEXCEPT;

	/*
	*	Returns the amount of seconds since this time point.
	*/
	NO_DISCARD float64 GetSecondsSince() const NOEXCEPT;

	/*
	*	Adds seconds.
	*/
	void AddSeconds(const float64 value) NOEXCEPT;

private:

	//The implementation.
	Any<8> _Implementation;

};