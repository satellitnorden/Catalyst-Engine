#pragma once

template <class FirstType, class SecondType>
class Pair final
{

public:

	/*
	*	Default constructor.
	*/
	Pair() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	Pair(const FirstType &initialFirstValue, const SecondType &initialSecondValue) NOEXCEPT
		:
		firstValue(initialFirstValue),
		secondValue(initialSecondValue)
	{

	}

	/*
	*	Default destructor.
	*/
	~Pair() NOEXCEPT
	{

	}

	/*
	*	Returns the first value, const.
	*/
	const FirstType& GetFirstValue() const NOEXCEPT { return firstValue; }

	/*
	*	Returns the first value, non-const.
	*/
	FirstType& GetFirstValue() NOEXCEPT { return firstValue; }

	/*
	*	Returns the second value, const.
	*/
	const SecondType& GetSecondValue() const NOEXCEPT { return secondValue; }

	/*
	*	Returns the second value, non-const.
	*/
	SecondType& GetSecondValue() NOEXCEPT { return secondValue; }

private:

	//The first value.
	FirstType firstValue;

	//The second valuye.
	SecondType secondValue;

};