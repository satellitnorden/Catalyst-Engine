#pragma once

template <typename FirstType, typename SecondType, typename ThirdType>
class Triple final
{

public:

	//The first value.
	FirstType _First;

	//The second value.
	SecondType _Second;

	//The third value.
	ThirdType _Third;

	/*
	*	Default constructor.
	*/
	constexpr Triple() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr Triple(const FirstType &initialFirst, const SecondType &initialSecond, const ThirdType &initialThird) NOEXCEPT
		:
		_First(initialFirst),
		_Second(initialSecond),
		_Third(initialThird)
	{

	}

};