#pragma once

template <class ObjectType>
class Optional
{

public:

	/*
	*	Default constructor.
	*/
	Optional() CATALYST_NOEXCEPT
		:
		object(nullptr)
	{

	}

	/*
	*	Constructor taking the underlying object by lvalue as an argument.
	*/
	Optional(const ObjectType &newObject) CATALYST_NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Constructor taking the underlying object by rvalue as an argument.
	*/
	Optional(ObjectType &&newObject) CATALYST_NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Arrow operator overload, const.
	*/
	CATALYST_RESTRICTED const ObjectType* operator->() const CATALYST_NOEXCEPT
	{
		return object;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	CATALYST_RESTRICTED ObjectType* operator->() CATALYST_NOEXCEPT
	{
		return object;
	}

	/*
	*	Bool operator overload.
	*/
	operator bool() const CATALYST_NOEXCEPT
	{
		return object != nullptr;
	}

	/*
	*	Dereference operator overload, const.
	*/
	const ObjectType& operator*() const CATALYST_NOEXCEPT
	{
		return *object;
	}

	/*
	*	Dereference operator overload, non-const.
	*/
	ObjectType& operator*() CATALYST_NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, const.
	*/
	const ObjectType& Get() const CATALYST_NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, non-const.
	*/
	ObjectType& Get() CATALYST_NOEXCEPT
	{
		return *object;
	}

private:

	//The underlying object.
	const ObjectType *CATALYST_RESTRICT const object;

};