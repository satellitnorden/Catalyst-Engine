#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename TYPE>
class Optional final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Optional() NOEXCEPT
		:
		_Value(),
		_Valid(false)
	{

	}

	/*
	*	Constructor taking the value as an argument.
	*/
	FORCE_INLINE constexpr Optional(const TYPE &initial_value) NOEXCEPT
		:
		_Value(initial_value),
		_Valid(true)
	{

	}

	/*
	*	Arrow operator overload.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT operator->() NOEXCEPT
	{
		return &_Value;
	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const TYPE &new_value) NOEXCEPT
	{
		new (this) Optional(new_value);
	}

	/*
	*	Bool conversion operator overload.
	*/
	FORCE_INLINE constexpr operator bool() NOEXCEPT
	{
		return _Valid;
	}

	/*
	*	Type conversion operator overload.
	*/
	FORCE_INLINE constexpr operator TYPE() NOEXCEPT
	{
		return _Value;
	}

	/*
	*	Resets this optional.
	*/
	FORCE_INLINE constexpr void Reset() NOEXCEPT
	{
		new (this) Optional();
	}

private:

	//The value.
	TYPE _Value;

	//Denotes whether or not this optional holds a value.
	bool _Valid{ false };

};