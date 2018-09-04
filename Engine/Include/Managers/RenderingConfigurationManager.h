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
	*	Returns the bloom radius.
	*/
	float GetBloomRadius() const NOEXCEPT { return _BloomRadius; }

	/*
	*	Sets the bloom radius.
	*/
	void SetBloomRadius(const float newBloomRadius) NOEXCEPT { _BloomRadius = newBloomRadius; }

	/*
	*	Returns the bloom strength.
	*/
	float GetBloomStrength() const NOEXCEPT { return _BloomStrength; }

	/*
	*	Sets the bloom strength.
	*/
	void SetBloomStrength(const float newBloomStrength) NOEXCEPT { _BloomStrength = newBloomStrength; }

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return _SaturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT { _SaturationStrength = newSaturationStrength; }

	/*
	*	Returns whether or not screen space ambient occlusion is enabled.
	*/
	bool GetScreenSpaceAmbientOcclusionEnabled() const NOEXCEPT { return _ScreenSpaceAmbientOcclusionEnabled; }

	/*
	*	Sets whether or not screen space ambient occlusion is enabled.
	*/
	void SetScreenSpaceAmbientOcclusionEnabled(const bool newScreenSpaceAmbientOcclusionEnabled) NOEXCEPT { _ScreenSpaceAmbientOcclusionEnabled = newScreenSpaceAmbientOcclusionEnabled; }

private:

	//Denotes the bloom radius.
	float _BloomRadius{ 0.001f };

	//The bloom strength.
	float _BloomStrength{ 0.0f };

	//The saturation strength.
	float _SaturationStrength{ 1.0f };

	//Denotes whether or not screen space ambient occlusion is enabled.
	bool _ScreenSpaceAmbientOcclusionEnabled{ true };

};