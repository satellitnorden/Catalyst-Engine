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
	*	Returns whether or not screen space ambient occlusion is enabled.
	*/
	bool GetScreenSpaceAmbientOcclusionEnabled() const NOEXCEPT { return screenSpaceAmbientOcclusionEnabled; }

	/*
	*	Sets whether or not screen space ambient occlusion is enabled.
	*/
	void SetScreenSpaceAmbientOcclusionEnabled(const bool newScreenSpaceAmbientOcclusionEnabled) NOEXCEPT { screenSpaceAmbientOcclusionEnabled = newScreenSpaceAmbientOcclusionEnabled; }

private:

	//Denotes whether or not screen space ambient occlusion is enabled.
	bool screenSpaceAmbientOcclusionEnabled{ true };

};