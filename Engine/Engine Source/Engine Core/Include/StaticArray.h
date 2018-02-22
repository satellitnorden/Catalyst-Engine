#pragma once

template <class ObjectType, size_t ArraySize>
class StaticArray final
{

public:

	/*
	*	Default constructor.
	*/
	StaticArray() NOEXCEPT
	{

	}

	/*
	*	Constructor taking a variadic number of arguments.
	*/
	template <class... Arguments>
	constexpr StaticArray(Arguments&&... arguments)
		:
		array{ arguments... }
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	const ObjectType& operator[](const size_t index) const NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	ObjectType& operator[](const size_t index) NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const ObjectType* begin() const NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED ObjectType* begin()  NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const ObjectType* end() const NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED ObjectType* end() NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	RESTRICTED const ObjectType* const Data() const NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	RESTRICTED ObjectType* Data() NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns the size of this array.
	*/
	size_t Size() const NOEXCEPT
	{
		return ArraySize;
	}

private:

	//The underlying array.
	ObjectType array[ArraySize];

};