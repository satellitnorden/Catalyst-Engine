#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
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
	*	Constructor taking a static array.
	*/
	template <uint64 LENGTH>
	ArrayProxy(const StaticArray<Type, LENGTH> &staticArray) NOEXCEPT
		:
		_Array(staticArray.begin()),
		_Length(staticArray.Size())
	{

	}

};