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
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const TYPE &new_value) NOEXCEPT
	{
		new (this) Optional(new_value);
	}

	/*
	*	Bool conversion operator overload.
	*/
	FORCE_INLINE constexpr operator bool() const NOEXCEPT
	{
		return _Valid;
	}

	/*
	*	Type conversion operator overload.
	*/
	FORCE_INLINE constexpr operator TYPE() const NOEXCEPT
	{
		return _Value;
	}

	/*
	*	Returns the underlying value, const.
	*/
	FORCE_INLINE NO_DISCARD const TYPE& Get() const NOEXCEPT
	{
		return _Value;
	}

	/*
	*	Returns the underlying value, non-const.
	*/
	FORCE_INLINE NO_DISCARD TYPE& Get() NOEXCEPT
	{
		return _Value;
	}

private:

	//The value.
	TYPE _Value;

	//Denotes whether or not this optional holds a value.
	bool _Valid{ false };

};

template <typename TYPE>
class OptionalWithDefault final
{

public:


	/*
	*	Constructor taking the default value as an argument.
	*/
	FORCE_INLINE constexpr OptionalWithDefault(const TYPE& initial_value) NOEXCEPT
		:
		_DefaultValue(initial_value),
		_Valid(false)
	{

	}

	/*
	*	Assignment operator overload.
	*/
	FORCE_INLINE constexpr void operator=(const TYPE& new_value) NOEXCEPT
	{
		_ActualValue = new_value;
		_Valid = true;
	}

	/*
	*	Type conversion operator overload.
	*/
	FORCE_INLINE constexpr operator TYPE() const NOEXCEPT
	{
		return _Valid ? _ActualValue : _DefaultValue;
	}

private:

	//The default value.
	TYPE _DefaultValue;

	//The actual value.
	TYPE _ActualValue;

	//Denotes whether or not this optional holds a value.
	bool _Valid{ false };

};