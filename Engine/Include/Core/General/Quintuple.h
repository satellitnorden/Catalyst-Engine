#pragma once

template <typename FirstType, typename SecondType, typename ThirdType, typename FourthType, typename FifthType>
class Quintuple final
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

	//The fifth value.
	FifthType _Fifth;

	/*
	*	Default constructor.
	*/
	constexpr Quintuple() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	constexpr Quintuple(const FirstType &initialFirst, const SecondType &initialSecond, const ThirdType &initialThird, const FourthType &initialFourth, const FifthType &initialFifth) NOEXCEPT
		:
		_First(initialFirst),
		_Second(initialSecond),
		_Third(initialThird),
		_Fourth(initialFourth),
		_Fifth(initialFifth)
	{

	}

};