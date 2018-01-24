#pragma once

template <class FirstType, class SecondType>
class Pair final
{

public:

	/*
	*	Default constructor.
	*/
	Pair() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	Pair(const FirstType &initialFirstValue, const SecondType &initialSecondValue) CATALYST_NOEXCEPT
		:
		firstValue(initialFirstValue),
		secondValue(initialSecondValue)
	{

	}

	/*
	*	Default destructor.
	*/
	~Pair() CATALYST_NOEXCEPT
	{

	}

	/*
	*	Returns the first value, const.
	*/
	const FirstType& GetFirstValue() const CATALYST_NOEXCEPT { return firstValue; }

	/*
	*	Returns the first value, non-const.
	*/
	FirstType& GetFirstValue() CATALYST_NOEXCEPT { return firstValue; }

	/*
	*	Returns the second value, const.
	*/
	const SecondType& GetSecondValue() const CATALYST_NOEXCEPT { return secondValue; }

	/*
	*	Returns the second value, non-const.
	*/
	SecondType& GetSecondValue() CATALYST_NOEXCEPT { return secondValue; }

private:

	//The first value.
	FirstType firstValue;

	//The second valuye.
	SecondType secondValue;

};