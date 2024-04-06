#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

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
	*	Multiplication by float32 operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD Resolution operator*(const float32 scalar) NOEXCEPT
	{
		return Resolution(BaseMath::Round<uint32>(static_cast<float32>(_Width) * scalar), BaseMath::Round<uint32>(static_cast<float32>(_Height) * scalar));
	}

	/*
	*	Multiplication by uint32 operator overload.
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