#pragma once

template <class ObjectType>
class UniquePointer
{

public:

	/*
	*	Default constructor, prohibied - must be constructed with the proper arguments.
	*/
	CATALYST_NOALIAS UniquePointer() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking the underlying pointer as the argument.
	*/
	CATALYST_NOALIAS UniquePointer(ObjectType *const CATALYST_RESTRICT newPointer) CATALYST_NOEXCEPT
		:
		pointer(newPointer)
	{

	}

	/*
	*	Default destructor.
	*/
	CATALYST_NOALIAS ~UniquePointer() CATALYST_NOEXCEPT
	{
		//Delete the underlying pointer.
		delete pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_FORCE_INLINE CATALYST_NOALIAS CATALYST_RESTRICTED const ObjectType* operator->() const CATALYST_NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_FORCE_INLINE CATALYST_NOALIAS CATALYST_RESTRICTED ObjectType* operator->() CATALYST_NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	ObjectType *CATALYST_RESTRICT pointer{ nullptr };

};