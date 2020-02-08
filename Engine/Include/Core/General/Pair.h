#pragma once

template <typename FirstType, typename SecondType>
class Pair final
{

public:

	//The first value.
	FirstType _First;

	//The second value.
	SecondType _Second;

	/*
	*	Default constructor.
	*/
	constexpr Pair() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr Pair(const FirstType &initial_first, const SecondType &initial_second) NOEXCEPT
		:
		_First(initial_first),
		_Second(initial_second)
	{

	}

};