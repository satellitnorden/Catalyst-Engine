#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace CatalystEngineResourceBuilding
{

	/*
	*	Builds resources for the Catalyst Engine.
	*/
	void BuildResources() NOEXCEPT
#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
		;
#else
	{

	}
#endif

#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
	/*
	*	Builds the cloud texture.
	*/
	void BuildCloudTexture();

	/*
	*	Builds the ocean texture.
	*/
	void BuildOceanTexture();
#endif

}