#pragma once

template <typename Type>
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
	Optional(const Type &newObject) NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Constructor taking the underlying object by rvalue as an argument.
	*/
	Optional(Type &&newObject) NOEXCEPT
		:
		object(newObject)
	{

	}

	/*
	*	Arrow operator overload, const.
	*/
	RESTRICTED const Type *const RESTRICT operator->() const NOEXCEPT
	{
		return object;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED Type *const RESTRICT operator->() NOEXCEPT
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
	const Type& operator*() const NOEXCEPT
	{
		return *object;
	}

	/*
	*	Dereference operator overload, non-const.
	*/
	Type& operator*() NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, const.
	*/
	const Type& Get() const NOEXCEPT
	{
		return *object;
	}

	/*
	*	Returns the underlying object, non-const.
	*/
	Type& Get() NOEXCEPT
	{
		return *object;
	}

private:

	//The underlying object.
	const Type *const RESTRICT object;

};