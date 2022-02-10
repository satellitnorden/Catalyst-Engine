#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename TYPE, uint64 SIZE>
class StaticArray final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr StaticArray() NOEXCEPT
		:
		_Array{ }
	{

	}

	/*
	*	Constructor taking a variadic number of arguments.
	*/
	template <class... Arguments>
	FORCE_INLINE constexpr StaticArray(Arguments&&... arguments)
		:
		_Array{ arguments... }
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	FORCE_INLINE constexpr NO_DISCARD const TYPE& operator[](const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	FORCE_INLINE constexpr NO_DISCARD TYPE& operator[](const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE *const RESTRICT Begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT Begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE *const RESTRICT End() const NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT End() NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	Begin iterator, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE *const RESTRICT begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE *const RESTRICT end() const NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT end() NOEXCEPT
	{
		return _Array + SIZE;
	}

	/*
	*	Returns the element at the given index, const.
	*/
	FORCE_INLINE constexpr NO_DISCARD const TYPE& At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the given index, non-const.
	*/
	FORCE_INLINE constexpr NO_DISCARD TYPE& At(const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD const TYPE *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the size of this array.
	*/
	FORCE_INLINE constexpr NO_DISCARD uint64 Size() const NOEXCEPT
	{
		return SIZE;
	}

	/*
	*	Returns the last index if this dynamic array.
	*/
	FORCE_INLINE constexpr NO_DISCARD uint64 LastIndex() const NOEXCEPT
	{
		return SIZE - 1;
	}

private:

	//The underlying array.
	TYPE _Array[SIZE];

};