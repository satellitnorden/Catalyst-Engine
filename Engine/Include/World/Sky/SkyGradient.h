#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class SkyGradient final
{

public:

	//The upper sky color.
	Vector3<float> _UpperSkyColor;

	//The lower sky color.
	Vector3<float> _LowerSkyColor;

	/*
	*	Default constructor.
	*/
	FORCE_INLINE constexpr SkyGradient() NOEXCEPT
	{

	}

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr SkyGradient(const Vector3<float> &initial_upper_sky_color, const Vector3<float> &initial_lower_sky_color) NOEXCEPT
		:
		_UpperSkyColor(initial_upper_sky_color),
		_LowerSkyColor(initial_lower_sky_color)
	{

	}

	/*
	*	Addition by sky gradient operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD SkyGradient operator+(const SkyGradient &other) const NOEXCEPT
	{
		return SkyGradient(_UpperSkyColor + other._UpperSkyColor, _LowerSkyColor + other._LowerSkyColor);
	}

	/*
	*	Multiplication by scalar operator overload.
	*/
	FORCE_INLINE constexpr NO_DISCARD SkyGradient operator*(const float scalar) const NOEXCEPT
	{
		return SkyGradient(_UpperSkyColor * scalar, _LowerSkyColor * scalar);
	}

};