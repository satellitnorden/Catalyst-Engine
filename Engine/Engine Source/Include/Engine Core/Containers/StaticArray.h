#pragma once

template <class ObjectType, uint64 ArraySize>
class StaticArray final
{

public:

	/*
	*	Default constructor.
	*/
	constexpr StaticArray() NOEXCEPT
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
	constexpr const ObjectType& operator[](const uint64 index) const NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	constexpr ObjectType& operator[](const uint64 index) NOEXCEPT
	{
		return array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED constexpr const ObjectType *const RESTRICT begin() const NOEXCEPT
	{
		return array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED constexpr ObjectType *const RESTRICT begin()  NOEXCEPT
	{
		return array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED constexpr const ObjectType *const RESTRICT end() const NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED constexpr ObjectType *const RESTRICT end() NOEXCEPT
	{
		return array + ArraySize;
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	constexpr const ObjectType *const RESTRICT Data() const NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	constexpr ObjectType *RESTRICT Data() NOEXCEPT
	{
		return array;
	}

	/*
	*	Returns the size of this array.
	*/
	constexpr uint64 Size() const NOEXCEPT
	{
		return ArraySize;
	}

private:

	//The underlying array.
	ObjectType array[ArraySize];

};