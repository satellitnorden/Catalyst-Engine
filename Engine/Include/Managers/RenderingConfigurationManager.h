#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

class RenderingConfigurationManager final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(RenderingConfigurationManager);

	/*
	*	Default constructor.
	*/
	RenderingConfigurationManager() NOEXCEPT { }

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return _SaturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT { _SaturationStrength = newSaturationStrength; }

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	/*
	*	Returns the volumetric fog density.
	*/
	float GetVolumetricFogDensity() const NOEXCEPT { return _VolumetricFogDensity; }

	/*
	*	Sets the volumetric fog density.
	*/
	void SetVolumetricFogDensity(const float newVolumetricFogDensity) NOEXCEPT { _VolumetricFogDensity = newVolumetricFogDensity; }
#endif

private:

	//The saturation strength.
	float _SaturationStrength{ 1.0f };

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	//The volumetric fog density.
	float _VolumetricFogDensity{ 1.0f };
#endif

};