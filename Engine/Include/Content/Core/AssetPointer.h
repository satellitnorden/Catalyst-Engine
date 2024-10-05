#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Content.
#include <Content/Core/Asset.h>

//Asset pointer data.
namespace AssetPointerData
{
	extern bool _DoReferenceCounting;
}

/*
*	Wrapper around a pointer to an Asset, or any derived classes from it.
*	Implements reference-counting to determine when to load/unload resources,
*	so copies should ONLY be made when the reference count should be incremented.
*/
template <typename TYPE>
class AssetPointer final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE AssetPointer() NOEXCEPT
		:
		_Asset(nullptr)
	{
	
	}

	/*
	*	Constructor taking the underlying resource as an argument.
	*/
	FORCE_INLINE AssetPointer(TYPE *const RESTRICT asset) NOEXCEPT
		:
		_Asset(asset)
	{
		IncrementReferenceCount();
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~AssetPointer() NOEXCEPT
	{
		DecrementReferenceCount();
	}

	/*
	*	Copy operator overload.
	*/
	FORCE_INLINE void operator=(const AssetPointer &other) NOEXCEPT
	{
		DecrementReferenceCount();
		_Asset = other._Asset;
		IncrementReferenceCount();
	}

	/*
	*	Arrow operator overload, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TYPE *const RESTRICT operator->() const NOEXCEPT
	{
		return _Asset;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT operator->() NOEXCEPT
	{
		return _Asset;
	}

	/*
	*	Bool operator overload.
	*/
	FORCE_INLINE operator bool() const NOEXCEPT
	{
		return _Asset != nullptr;
	}

	/*
	*	Returns the underlying asset, const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TYPE *const RESTRICT Get() const NOEXCEPT
	{
		return _Asset;
	}

	/*
	*	Returns the underlying asset, non-const.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT Get() NOEXCEPT
	{
		return _Asset;
	}

private:

	//The underlying asset.
	TYPE *RESTRICT _Asset;

	/*
	*	Increments the reference count.
	*/
	FORCE_INLINE void IncrementReferenceCount() const NOEXCEPT
	{
		if (AssetPointerData::_DoReferenceCounting && _Asset)
		{
			++_Asset->_ReferenceCount;
		}
	}

	/*
	*	Decrements the reference count.
	*/
	FORCE_INLINE void DecrementReferenceCount() const NOEXCEPT
	{
		if (AssetPointerData::_DoReferenceCounting && _Asset)
		{
			--_Asset->_ReferenceCount;
		}
	}

};