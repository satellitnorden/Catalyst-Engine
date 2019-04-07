#pragma once

template <typename Type>
class UniquePointer final
{

public:

	/*
	*	Default constructor, prohibied - must be constructed with the proper arguments.
	*/
	UniquePointer() NOEXCEPT = delete;

	/*
	*	Constructor taking the underlying pointer as the argument.
	*/
	UniquePointer(Type *const RESTRICT newPointer) NOEXCEPT
		:
		pointer(newPointer)
	{

	}

	/*
	*	Default destructor.
	*/
	~UniquePointer() NOEXCEPT
	{
		//Delete the underlying pointer.
		delete pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED const Type *const RESTRICT operator->() const NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	RESTRICTED Type *const RESTRICT operator->() NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Returns the underlying pointer, const.
	*/
	RESTRICTED const Type *const RESTRICT Get() const NOEXCEPT
	{
		return pointer;
	}

	/*
	*	Returns the underlying pointer, non-const.
	*/
	RESTRICTED Type *const RESTRICT Get() NOEXCEPT
	{
		return pointer;
	}

private:

	//The underlying pointer.
	Type *RESTRICT pointer;

};