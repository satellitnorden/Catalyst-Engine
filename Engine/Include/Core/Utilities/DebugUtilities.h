#if defined(CATALYST_CONFIGURATION_DEBUG)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace DebugUtilities
{

	/*
	*	Debug renders all axis aligned bounding boxes for grass vegetation.
	*/
	void DebugRenderGrassVegetationAxisAlignedBoundingBoxes() NOEXCEPT;

	/*
	*	Debug renders all axis aligned bounding boxes for solid vegetation.
	*/
	void DebugRenderSolidVegetationAxisAlignedBoundingBoxes() NOEXCEPT;

	/*
	*	Debug renders all axis aligned bounding boxes for terrain.
	*/
	void DebugRenderTerrainAxisAlignedBoundingBoxes() NOEXCEPT;

}
#endif