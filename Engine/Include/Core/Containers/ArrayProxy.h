#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

template <typename Type>
class ArrayProxy final
{

public:

	//Pointer to the beginning of the array.
	Type *const RESTRICT array;

	//The length of the array.
	uint64 length;

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
		array(staticArray.begin()),
		length(staticArray.Size())
	{

	}

};