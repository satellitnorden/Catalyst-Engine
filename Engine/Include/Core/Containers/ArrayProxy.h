#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/Containers/StaticArray.h>

template <typename Type>
class ArrayProxy final
{

public:

	//Pointer to the beginning of the array.
	Type *const RESTRICT _Array;

	//The length of the array.
	uint64 _Length;

	/*
	*	Default constructor, prohibited - must be constructed with the proper arguments.
	*/
	ArrayProxy() NOEXCEPT = delete;

	/*
	*	Constructor taking a dynamic array.
	*/
	ArrayProxy(const DynamicArray<Type> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Length(array.Size())
	{

	}

	/*
	*	Constructor taking a static array.
	*/
	template <uint64 LENGTH>
	ArrayProxy(const StaticArray<Type, LENGTH> &array) NOEXCEPT
		:
		_Array(array.Data()),
		_Length(array.Size())
	{

	}

};