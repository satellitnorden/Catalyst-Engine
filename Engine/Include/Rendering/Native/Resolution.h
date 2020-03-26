#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class Resolution final
{

public:

	//The width.
	uint32 _Width;

	//The height.
	uint32 _Height;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr Resolution() NOEXCEPT
		:
		_Width(0),
		_Height(0)
	{
	
	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr Resolution(const uint32 initial_width, const uint32 initial_height) NOEXCEPT
		:
		_Width(initial_width),
		_Height(initial_height)
	{

	}

	/*
	*	Division operator overload.	
	*/
		FORCE_INLINE constexpr NO_DISCARD Resolution operator/(const uint32 scalar) NOEXCEPT
	{
		return Resolution(_Width / scalar, _Height / scalar);
	}

	/*
	*	Multiplication operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Resolution operator*(const uint32 scalar) NOEXCEPT
	{
		return Resolution(_Width * scalar, _Height * scalar);
	}

	/*
	*	Rounds the resolution up to the nearest multiple of two in both dimensions.
	*/
	FORCE_INLINE constexpr void RoundUpToNearestMultipleOfTwo() NOEXCEPT
	{
		if ((_Width & 1) != 0)
		{
			++_Width;
		}

		if ((_Height & 1) != 0)
		{
			++_Height;
		}
	}

};