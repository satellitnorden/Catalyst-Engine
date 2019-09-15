#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class EnvironmentManager final
{

public:

	/*
	*	Returns the upper sky color.
	*/
	FORCE_INLINE static NO_DISCARD const Vector3<float>& GetUpperSkyColor() NOEXCEPT
	{
		return GetUpperSkyColorInternal();
	}

	/*
	*	Sets the upper sky color.
	*/
	FORCE_INLINE static void SetUpperSkyColor(const Vector3<float> &color) NOEXCEPT
	{
		GetUpperSkyColorInternal() = color;
	}

	/*
	*	Returns the lower sky color.
	*/
	FORCE_INLINE static NO_DISCARD const Vector3<float>& GetLowerSkyColor() NOEXCEPT
	{
		return GetLowerSkyColorInternal();
	}

	/*
	*	Sets the lower sky color.
	*/
	FORCE_INLINE static void SetLowerSkyColor(const Vector3<float> &color) NOEXCEPT
	{
		GetLowerSkyColorInternal() = color;
	}

	/*
	*	Returns the star strength.
	*/
	FORCE_INLINE static NO_DISCARD float GetStarStrength() NOEXCEPT
	{
		return GetStarStrengthInternal();
	}

	/*
	*	Sets the star strength.
	*/
	FORCE_INLINE static void SetStarStrength(const float strength) NOEXCEPT
	{
		GetStarStrengthInternal() = strength;
	}

private:

	/*
	*	Returns the upper sky color.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float>& GetUpperSkyColorInternal() NOEXCEPT
	{
		static Vector3<float> color;

		return color;
	}

	/*
	*	Returns the lower sky color.
	*/
	FORCE_INLINE static NO_DISCARD Vector3<float>& GetLowerSkyColorInternal() NOEXCEPT
	{
		static Vector3<float> color;

		return color;
	}

	/*
	*	Returns the star strength.
	*/
	FORCE_INLINE static NO_DISCARD float& GetStarStrengthInternal() NOEXCEPT
	{
		static float strength;

		return strength;
	}

};