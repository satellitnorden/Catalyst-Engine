#if !defined(CATALYST_FINAL)
#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

namespace DebugUtilities
{

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