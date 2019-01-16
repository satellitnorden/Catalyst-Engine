#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

template <typename TYPE>
class ArrayProxy final
{

public:

	//Pointer to the beginning of the array.
	const TYPE *RESTRICT _Array;

	//The size of the array.
	uint64 _Size;

	/*
	*	Default constructor.
	*/
	ArrayProxy() NOEXCEPT
		:
		_Array(nullptr),
		_Size(0)
	{

	}

	/*
	*	Constructor taking a single value.
	*/
	ArrayProxy(const TYPE &value) NOEXCEPT
		:
		_Array(&value),
		_Size(1)
	{

	}

	/*
	*	Constructor taking a dynamic array.
	*/
	ArrayProxy(const DynamicArray<TYPE> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Size(array.Size())
	{

	}

	/*
	*	Constructor taking a static array.
	*/
	template <uint64 LENGTH>
	ArrayProxy(const StaticArray<TYPE, LENGTH> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Size(array.Size())
	{

	}

	/*
	*	Assignment operator overload taking a single value.
	*/
	void operator=(const TYPE &value) NOEXCEPT
	{
		new (this) ArrayProxy(value);
	}

	/*
	*	Assignment operator overload taking a dynamic array.
	*/
	void operator=(const DynamicArray<TYPE> &array) NOEXCEPT
	{
		new (this) ArrayProxy(array);
	}

	/*
	*	Assignment operator overload taking a static array.
	*/
	template <uint64 LENGTH>
	void operator=(const StaticArray<TYPE, LENGTH> &array) NOEXCEPT
	{
		new (this) ArrayProxy(array);
	}

	/*
	*	Subscript operator overload, const.
	*/
	const TYPE& operator[](const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	TYPE& operator[](const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT Begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT Begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT End() const NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT End() NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	Begin iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT begin() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Begin iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT begin()  NOEXCEPT
	{
		return _Array;
	}

	/*
	*	End iterator, const.
	*/
	RESTRICTED const TYPE *const RESTRICT end() const NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	End iterator, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT end() NOEXCEPT
	{
		return _Array + _Size;
	}

	/*
	*	Returns the element at the given index, const.
	*/
	const TYPE& At(const uint64 index) const NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns the element at the given index, non-const.
	*/
	TYPE& At(const uint64 index) NOEXCEPT
	{
		return _Array[index];
	}

	/*
	*	Returns a pointer to the data of the array, const.
	*/
	RESTRICTED const TYPE *const RESTRICT Data() const NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns a pointer to the data of the array, non-const.
	*/
	RESTRICTED TYPE *const RESTRICT Data() NOEXCEPT
	{
		return _Array;
	}

	/*
	*	Returns the size of this array.
	*/
	uint64 Size() const NOEXCEPT
	{
		return _Size;
	}

};