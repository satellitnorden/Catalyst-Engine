#pragma once

template <typename FirstType, typename SecondType>
class Pair final
{

public:

	//The first value.
	FirstType first;

	//The second valuye.
	SecondType second;

	/*
	*	Default constructor.
	*/
	Pair() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	Pair(const FirstType &initialFirst, const SecondType &initialSecond) NOEXCEPT
		:
		first(initialFirst),
		second(initialSecond)
	{

	}

};