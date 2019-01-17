#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Math.
#include <Math/Matrix4.h>

//Rendering.
#include <Rendering/Engine/AxisAlignedBoundingBox.h>

//Type aliases.
using VegetationPlacementFunction = bool(*)(const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation);

//Enumeration covering all vegetation types.
enum class VegetationType : uint8
{
	Debris,
	Grass,
	Solid,

	NumberOfVegetationTypes
};

//Enumeration covering all grass vegetion level of details.
enum class GrassVegetationLevelOfDetail : uint8
{
	Low,
	High,

	NumberOfGrassVegetationLevelOfDetails
};