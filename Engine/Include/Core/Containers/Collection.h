#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

template <typename Type>
class Collection
{
	
public:

	/*
	*	Default constructor.
	*/
	Collection() NOEXCEPT
	{
		//Add this instance to the container for all instances of this type.
		_Instances.EmplaceSlow(static_cast<Type *RESTRICT>(this));
	}

	/*
	*	Default destructor.
	*/
	virtual ~Collection() NOEXCEPT
	{
		//Remove this instance from the container for all instances of this type.
		_Instances.Erase(static_cast<Type *RESTRICT>(this));
	}

	/*
	*	Returns the first instance of this type.
	*/
	NO_DISCARD RESTRICTED static Type *const RESTRICT First() NOEXCEPT
	{
		return _Instances.Begin();
	}

	/*
	*	Returns the last instance of this type.
	*/
	NO_DISCARD RESTRICTED static Type *const RESTRICT Last() NOEXCEPT
	{
		return _Instances.End();
	}

private:

	//The container for all instances of this type.
	static DynamicArray<Type *RESTRICT> _Instances;

};

//Static variable definitions.
template <typename Type>
DynamicArray<Type *RESTRICT> Collection<Type>::_Instances;