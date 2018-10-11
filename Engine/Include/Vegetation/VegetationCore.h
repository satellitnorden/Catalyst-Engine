#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//Vegetation constants.
namespace VegetationConstants
{
	constexpr float VEGETATION_GRID_SIZE{ 1.25f };
}

//Type aliases.
using VegetationPlacementFunction = bool(*)(const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation);

//Enumeration covering all vegetation types.
enum class VegetationType : uint8
{
	Grass,
	Solid,

	NumberOfVegetationTypes
};