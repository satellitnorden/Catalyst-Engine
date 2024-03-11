#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

namespace CatalystEngineResourceBuilding
{

	/*
	*	Builds resources for the Catalyst Engine.
	*/
	void BuildResources(const CatalystProjectConfiguration &configuration) NOEXCEPT
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
	*	Builds the default texture 3D.
	*/
	void BuildDefaultTexture3D() NOEXCEPT;
#endif

}