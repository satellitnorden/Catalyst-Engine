#pragma once

template <class ObjectType>
class Optional final
{

public:

	/*
	*	Default constructor.
	*/
	Optional() NOEXCEPT
		:
		object(nullptr)
	{

	}

	/*
	*	Constructor taking the underlying object by lvalue as an argument.
	*/
	Optional(const ObjectType &newObject) NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Constructor taking the underlying object by rvalue as an argument.
	*/
	Optional(ObjectType &&newObject) NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Arrow operator overload, const.
	*/
	RESTRICTED const ObjectType* operator->() const NOEXCEPT
	{
		return object;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED ObjectType* operator->() NOEXCEPT
	{
		return object;
	}

	/*
	*	Bool operator overload.
	*/
	operator bool() const NOEXCEPT
	{
		return object != nullptr;
	}

	/*
	*	Dereference operator overload, const.
	*/
	const ObjectType& operator*() const NOEXCEPT
	{
		return *object;
	}

	/*
	*	Dereference operator overload, non-const.
	*/
	ObjectType& operator*() NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, const.
	*/
	const ObjectType& Get() const NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, non-const.
	*/
	ObjectType& Get() NOEXCEPT
	{
		return *object;
	}

private:

	//The underlying object.
	const ObjectType *RESTRICT const object;

};