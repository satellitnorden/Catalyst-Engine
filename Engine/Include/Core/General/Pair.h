#pragma once

template <typename FirstType, typename SecondType>
class Pair final
{

public:

	//The first value.
	FirstType _First;

	//The second valuye.
	SecondType _Second;

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
		_First(initialFirst),
		_Second(initialSecond)
	{

	}

};