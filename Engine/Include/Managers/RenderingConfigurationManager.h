#pragma once

//Core.
#include <Core/General/EngineCore.h>

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
	float GetBloomRadius() const NOEXCEPT { return bloomRadius; }

	/*
	*	Sets the bloom radius.
	*/
	void SetBloomRadius(const float newBloomRadius) NOEXCEPT { bloomRadius = newBloomRadius; }

	/*
	*	Returns the bloom strength.
	*/
	float GetBloomStrength() const NOEXCEPT { return bloomStrength; }

	/*
	*	Sets the bloom strength.
	*/
	void SetBloomStrength(const float newBloomStrength) NOEXCEPT { bloomStrength = newBloomStrength; }

	/*
	*	Returns the saturation strength.
	*/
	float GetSaturationStrength() const NOEXCEPT { return saturationStrength; }

	/*
	*	Sets the saturation strength.
	*/
	void SetSaturationStrength(const float newSaturationStrength) NOEXCEPT { saturationStrength = newSaturationStrength; }

	/*
	*	Returns whether or not screen space ambient occlusion is enabled.
	*/
	bool GetScreenSpaceAmbientOcclusionEnabled() const NOEXCEPT { return screenSpaceAmbientOcclusionEnabled; }

	/*
	*	Sets whether or not screen space ambient occlusion is enabled.
	*/
	void SetScreenSpaceAmbientOcclusionEnabled(const bool newScreenSpaceAmbientOcclusionEnabled) NOEXCEPT { screenSpaceAmbientOcclusionEnabled = newScreenSpaceAmbientOcclusionEnabled; }

private:

	//Denotes the bloom radius.
	float bloomRadius{ 0.001f };

	//The bloom strength.
	float bloomStrength{ 0.0f };

	//The saturation strength.
	float saturationStrength{ 1.0f };

	//Denotes whether or not screen space ambient occlusion is enabled.
	bool screenSpaceAmbientOcclusionEnabled{ true };

};