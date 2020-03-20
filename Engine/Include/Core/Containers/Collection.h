#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

template <typename TYPE>
class Collection
{
	
public:

	/*
	*	Default constructor.
	*/
	Collection() NOEXCEPT
	{
		//Add this instance to the container for all instances of this type.
		_Instances.EmplaceSlow(static_cast<TYPE *const RESTRICT>(this));
	}

	/*
	*	Default destructor.
	*/
	virtual ~Collection() NOEXCEPT
	{
		//Remove this instance from the container for all instances of this type.
		_Instances.Erase(static_cast<TYPE *const RESTRICT>(this));
	}

	/*
	*	Returns the first instance of this type.
	*/
	RESTRICTED static NO_DISCARD TYPE *const RESTRICT First() NOEXCEPT
	{
		return _Instances.Begin();
	}

	/*
	*	Returns the last instance of this type.
	*/
	RESTRICTED static NO_DISCARD TYPE *const RESTRICT Last() NOEXCEPT
	{
		return _Instances.End();
	}

private:

	//The container for all instances of this type.
	static DynamicArray<TYPE *RESTRICT> _Instances;

};

//Static variable definitions.
template <typename TYPE>
DynamicArray<TYPE *RESTRICT> Collection<TYPE>::_Instances;