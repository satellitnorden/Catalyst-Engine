#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class EnvironmentManager final
{

public:

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

	/*
	*	Returns the cloud density.
	*/
	FORCE_INLINE static NO_DISCARD float GetCloudDensity() NOEXCEPT
	{
		return GetCloudDensityInternal();
	}

	/*
	*	Sets the cloud density.
	*/
	FORCE_INLINE static void SetCloudDensity(const float density) NOEXCEPT
	{
		GetCloudDensityInternal() = density;
	}

private:

	/*
	*	Returns the star strength.
	*/
	FORCE_INLINE static NO_DISCARD float& GetStarStrengthInternal() NOEXCEPT
	{
		static float strength{ 0.0f };

		return strength;
	}

	/*
	*	Returns the cloud density.
	*/
	FORCE_INLINE static NO_DISCARD float& GetCloudDensityInternal() NOEXCEPT
	{
		static float density{ 0.5f };

		return density;
	}

};