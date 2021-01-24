#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

template <typename TYPE>
class ArrayProxy final
{

public:

	//Pointer to the beginning of the array.
	TYPE *RESTRICT _Array;

	//The size of the array.
	uint64 _Size;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr ArrayProxy() NOEXCEPT
		:
		_Array(nullptr),
		_Size(0)
	{

	}

	/*
	*	Constructor taking a single value.
	*/
	FORCE_INLINE constexpr ArrayProxy(TYPE &value) NOEXCEPT
		:
		_Array(&value),
		_Size(1)
	{

	}

	/*
	*	Constructor taking an array and size.
	*/
	FORCE_INLINE constexpr ArrayProxy(TYPE *const RESTRICT array, const uint64 size) NOEXCEPT
		:
		_Array(array),
		_Size(size)
	{

	}

	/*
	*	Constructor taking a dynamic array.
	*/
	FORCE_INLINE constexpr ArrayProxy(DynamicArray<TYPE> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Size(array.Size())
	{

	}

	/*
	*	Constructor taking a static array.
	*/
	template <uint64 LENGTH>
	FORCE_INLINE constexpr ArrayProxy(StaticArray<TYPE, LENGTH> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Size(array.Size())
	{

	}

	/*
	*	Assignment operator overload taking a single value.
	*/
	FORCE_INLINE constexpr void operator=(TYPE &value) NOEXCEPT
	{
		new (this) ArrayProxy(value);
	}

	/*
	*	Assignment operator overload taking a dynamic array.
	*/
	FORCE_INLINE constexpr void operator=(DynamicArray<TYPE> &array) NOEXCEPT
	{
		new (this) ArrayProxy(array);
	}

	/*
	*	Assignment operator overload taking a static array.
	*/
	template <uint64 LENGTH>
	FORCE_INLINE constexpr void operator=(StaticArray<TYPE, LENGTH> &array) NOEXCEPT
	{
		new (this) ArrayProxy(array);
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
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT End() NOEXCEPT
	{
		return _Array + _Size;
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
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	FORCE_INLINE RESTRICTED constexpr NO_DISCARD TYPE *const RESTRICT end() NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	Returns the element at the given index, const.
	*/
	FORCE_INLINE NO_DISCARD constexpr const TYPE& At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the given index, non-const.
	*/
	FORCE_INLINE NO_DISCARD constexpr TYPE& At(const uint64 index) NOEXCEPT
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
	*	Returns the back of this dynamic array, const.
	*/
	FORCE_INLINE const TYPE &Back() const NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Returns the back of this dynamic array, non-const.
	*/
	FORCE_INLINE TYPE &Back() NOEXCEPT
	{
		return _Array[LastIndex()];
	}

	/*
	*	Returns if the array is empty or not.
	*/
	FORCE_INLINE NO_DISCARD constexpr bool Empty() const NOEXCEPT
	{
		return _Size == 0;
	}

	/*
	*	Returns the last index if this dynamic array.
	*/
	FORCE_INLINE uint64 LastIndex() const NOEXCEPT
	{
		return _Size - 1;
	}

	/*
	*	Returns the size of this array.
	*/
	FORCE_INLINE NO_DISCARD constexpr uint64 Size() const NOEXCEPT
	{
		return _Size;
	}

};