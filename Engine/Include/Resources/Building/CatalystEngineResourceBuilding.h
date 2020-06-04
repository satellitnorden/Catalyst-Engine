#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace CatalystEngineResourceBuilding
{

	/*
	*	Builds resources for the Catalyst Engine.
	*/
	void BuildResources() NOEXCEPT
#if !defined(CATALYST_CONFIGURATION_FINAL)
		;
#else
	{

	}
#endif

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Builds the cloud texture.
	*/
	void BuildCloudTexture();

	/*
	*	Builds the ocean texture.
	*/
	void BuildOceanTexture();

	/*
	*	Builds the default sky texture.
	*/
	void BuildDefaultSkyTexture();
#endif

}