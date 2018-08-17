#pragma once

//Core.
#include <Core/EngineCore.h>

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

	//Denotes whether or not screen space ambient occlusion is enabled.
	bool screenSpaceAmbientOcclusionEnabled{ true };

};