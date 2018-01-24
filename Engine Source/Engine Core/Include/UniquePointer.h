#pragma once

template <class ObjectType>
class UniquePointer final
{

public:

	/*
	*	Default constructor, prohibied - must be constructed with the proper arguments.
	*/
	UniquePointer() CATALYST_NOEXCEPT = delete;

	/*
	*	Constructor taking the underlying pointer as the argument.
	*/
	UniquePointer(ObjectType *const CATALYST_RESTRICT newPointer) CATALYST_NOEXCEPT
		:
		pointer(newPointer)
	{

	}

	/*
	*	Default destructor.
	*/
	~UniquePointer() CATALYST_NOEXCEPT
	{
		//Delete the underlying pointer.
		delete pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_RESTRICTED const ObjectType* const operator->() const CATALYST_NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* operator->() CATALYST_NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	ObjectType *CATALYST_RESTRICT pointer;

};