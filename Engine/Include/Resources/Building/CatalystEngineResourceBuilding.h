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
	void BuildCloudTexture() NOEXCEPT;

	/*
	*	Builds the ocean texture.
	*/
	void BuildOceanTexture() NOEXCEPT;

	/*
	*	Builds the default sky texture.
	*/
	void BuildDefaultSkyTexture() NOEXCEPT;

	/*
	*	Builds the default texture 2D.
	*/
	void BuildDefaultTexture2D() NOEXCEPT;

	/*
	*	Builds the default texture 3D.
	*/
	void BuildDefaultTexture3D() NOEXCEPT;

	/*
	*	Builds the star texture.
	*/
	void BuildStarTexture() NOEXCEPT;
#endif

}