#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename Type, uint64 SIZE>
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
		_Array{ arguments... }
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	constexpr const Type& operator[](const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	constexpr Type& operator[](const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED constexpr const Type *const RESTRICT Begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED constexpr Type *const RESTRICT Begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED constexpr const Type *const RESTRICT End() const NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED constexpr Type *const RESTRICT End() NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED constexpr const Type *const RESTRICT begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED constexpr Type *const RESTRICT begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED constexpr const Type *const RESTRICT end() const NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED constexpr Type *const RESTRICT end() NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	Returns the element at the given index, const.
	*/
	constexpr const Type& At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the given index, non-const.
	*/
	constexpr Type& At(const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	RESTRICTED constexpr const Type *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	RESTRICTED constexpr Type *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the size of this array.
	*/
	constexpr uint64 Size() const NOEXCEPT
	{
		return SIZE;
	}

private:

	//The underlying array.
	Type _Array[SIZE];

};