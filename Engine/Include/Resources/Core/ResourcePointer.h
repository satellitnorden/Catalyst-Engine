#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/Resource.h>

/*
*	Wrapper around a pointer to a Resource, or any derived classes from it.
*	Implements reference-counting to determine when to load/unload resources,
*	so copies should ONLY be made when the reference count should be incremented.
*/
template <typename TYPE>
class ResourcePointer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE ResourcePointer() NOEXCEPT
		:
		_Resource(nullptr)
	{
	
	}

	/*
	*	Constructor taking the underlying resource as an argument.
	*/
	FORCE_INLINE ResourcePointer(TYPE *const RESTRICT initial_resource) NOEXCEPT
		:
		_Resource(initial_resource)
	{
		//Increment the resource's reference count.
		++_Resource->_ReferenceCount;
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ResourcePointer() NOEXCEPT
	{
		//Decrement the resource's reference count.
		if (_Resource)
		{
			--_Resource->_ReferenceCount;
		}
	}

	/*
	*	Arrow operator overload.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT operator->() NOEXCEPT
	{
		return _Resource;
	}

private:

	//The underlying resource.
	TYPE *RESTRICT _Resource;

};