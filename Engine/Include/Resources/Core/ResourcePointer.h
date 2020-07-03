#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Resources.
#include <Resources/Core/Resource.h>

//Resource pointer data.
namespace ResourcePointerData
{
	extern bool _DoReferenceCounting;
}

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
		IncrementReferenceCount();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~ResourcePointer() NOEXCEPT
	{
		DecrementReferenceCount();
	}

	/*
	*	Copy operator overload.
	*/
	FORCE_INLINE void operator=(const ResourcePointer &other) NOEXCEPT
	{
		DecrementReferenceCount();
		_Resource = other._Resource;
		IncrementReferenceCount();
	}

	/*
	*	Arrow operator overload, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TYPE *const RESTRICT operator->() const NOEXCEPT
	{
		return _Resource;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT operator->() NOEXCEPT
	{
		return _Resource;
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE operator bool() const NOEXCEPT
	{
		return _Resource != nullptr;
	}

private:

	//The underlying resource.
	TYPE *RESTRICT _Resource;

	/*
	*	Increments the reference count.
	*/
	FORCE_INLINE void IncrementReferenceCount() const NOEXCEPT
	{
		if (ResourcePointerData::_DoReferenceCounting && _Resource)
		{
			++_Resource->_ReferenceCount;
		}
	}

	/*
	*	Decrements the reference count.
	*/
	FORCE_INLINE void DecrementReferenceCount() const NOEXCEPT
	{
		if (ResourcePointerData::_DoReferenceCounting && _Resource)
		{
			--_Resource->_ReferenceCount;
		}
	}

};