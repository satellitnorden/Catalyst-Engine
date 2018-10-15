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

//Enumeration covering all vegetation level of details.
enum class VegetationLevelOfDetail : uint8
{
	Low = BIT(0),
	Medium = BIT(1),
	High = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(VegetationLevelOfDetail);

//Enumeration covering all vegetation types.
enum class VegetationType : uint8
{
	Grass,
	Solid,

	NumberOfVegetationTypes
};