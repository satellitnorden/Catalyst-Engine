#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/General/Vector.h>

class EnvironmentManager final
{

public:

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
	*	Returns the cloud density.
	*/
	FORCE_INLINE static NO_DISCARD float& GetCloudDensityInternal() NOEXCEPT
	{
		static float density{ 0.5f };

		return density;
	}

};