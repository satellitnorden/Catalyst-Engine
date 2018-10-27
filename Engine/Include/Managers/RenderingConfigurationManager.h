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

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	/*
	*	Returns the volumetric fog distance.
	*/
	float GetVolumetricFogDistance() const NOEXCEPT { return _VolumetricFogDistance; }

	/*
	*	Sets the volumetric fog distance.
	*/
	void SetVolumetricFogDistance(const float newVolumetricFogDistance) NOEXCEPT { _VolumetricFogDistance = newVolumetricFogDistance; }
#endif

private:

	//The depth of field distance.
	float _DepthOfFieldDistance{ 1'000.0f };

	//The saturation strength.
	float _SaturationStrength{ 1.0f };

#if defined(CATALYST_ENABLE_VOLUMETRIC_FOG)
	//The volumetric fog distance.
	float _VolumetricFogDistance{ 100.0f };
#endif

};