#pragma once

/*
*	Pointer that is responsible for calling the destructor in the memory it is handling.
*	Note that it does NOT free the underlying memory.
*/
template <typename TYPE>
class DestructorPointer final
{

public:

	/*
	*	Default constructor, prohibied - must be constructed with the proper arguments.
	*/
	DestructorPointer() NOEXCEPT = delete;

	/*
	*	Constructor taking the underlying pointer as the argument.
	*/
	DestructorPointer(TYPE *const RESTRICT initialPointer) NOEXCEPT
		:
		_Pointer(initialPointer)
	{

	}

	/*
	*	Default destructor.
	*/
	~DestructorPointer() NOEXCEPT
	{
		//Call the destructor on the underlying pointer.
		_Pointer->~TYPE();
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT operator->() const NOEXCEPT
	{
		return _Pointer;
	}

	/*
	*	Arrow operator overload, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT operator->() NOEXCEPT
	{
		return _Pointer;
	}

	/*
	*	Returns the underlying pointer, const.
	*/
	FORCE_INLINE RESTRICTED const TYPE *const RESTRICT Get() const NOEXCEPT
	{
		return _Pointer;
	}

	/*
	*	Returns the underlying pointer, non-const.
	*/
	FORCE_INLINE RESTRICTED TYPE *const RESTRICT Get() NOEXCEPT
	{
		return _Pointer;
	}

private:

	//The underlying pointer.
	TYPE *RESTRICT _Pointer;

};