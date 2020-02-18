#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//World.
#include <World/Environment/VolumetricLightingProperties.h>

class EnvironmentSystem final
{

public:

	/*
	*	Returns the volumetric lighting properties.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD VolumetricLightingProperties *const RESTRICT GetVolumetricLightingProperties() NOEXCEPT
	{
		return &_VolumetricLightingProperties;
	}

private:

	//The volumetric lighting properties.
	VolumetricLightingProperties _VolumetricLightingProperties;

};