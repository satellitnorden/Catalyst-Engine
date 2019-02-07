#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
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
	*	Returns the depth of field distance.
	*/
	float GetDepthOfFieldDistance() const NOEXCEPT { return _DepthOfFieldDistance; }

	/*
	*	Sets the depth of field distance.
	*/
	void SetDepthOfFieldDistanceh(const float newDepthOfFieldDistance) NOEXCEPT { _DepthOfFieldDistance = newDepthOfFieldDistance; }

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return _SaturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT { _SaturationStrength = newSaturationStrength; }

	/*
	*	Returns the volumetric fog density.
	*/
	float GetVolumetricFogDensity() const NOEXCEPT { return _VolumetricFogDensity; }

	/*
	*	Sets the volumetric fog density.
	*/
	void SetVolumetricFogDensity(const float newVolumetricFogDensity) NOEXCEPT { _VolumetricFogDensity = newVolumetricFogDensity; }

private:

	//The depth of field distance.
	float _DepthOfFieldDistance{ 1'000.0f };

	//The saturation strength.
	float _SaturationStrength{ 1.0f };

	//The volumetric fog density.
	float _VolumetricFogDensity{ 0.0f };

};