#pragma once

template <typename FirstType, typename SecondType, typename ThirdType, typename FourthType>
class Quadruple final
{

public:

	//The first value.
	FirstType _First;

	//The second value.
	SecondType _Second;

	//The third value.
	ThirdType _Third;

	//The fourth value.
	FourthType _Fourth;

	/*
	*	Default constructor.
	*/
	constexpr Quadruple() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr Quadruple(const FirstType &initialFirst, const SecondType &initialSecond, const ThirdType &initialThird, const FourthType &initialFourth) NOEXCEPT
		:
		_First(initialFirst),
		_Second(initialSecond),
		_Third(initialThird),
		_Fourth(initialFourth)
	{

	}

};