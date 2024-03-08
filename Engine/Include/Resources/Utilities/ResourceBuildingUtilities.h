#if defined(CATALYST_ENABLE_RESOURCE_BUILDING)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Math.
#include <Math/General/Matrix.h>

//Rendering.
#include <Rendering/Native/RenderingCore.h>

namespace ResourceBuildingUtilities
{

	/*
	*	Calculates the average color of the texture at the given file path.
	*/
	NO_DISCARD Vector4<float32> CalculateAverageColor(const char *const RESTRICT texture_file_path) NOEXCEPT;

}
#endif