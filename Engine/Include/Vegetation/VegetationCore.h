#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/AxisAlignedBoundingBox.h>
#include <Math/Matrix4.h>

//Type aliases.
using VegetationPlacementFunction = bool(*)(const AxisAlignedBoundingBox &box, Matrix4 *const RESTRICT transformation);

//Enumeration covering all vegetation types.
enum class VegetationType : uint8
{
	Debris,
	Grass,
	Solid,
	Tree,

	NumberOfVegetationTypes
};

//Enumeration covering all tree vegetation level of details.
enum class TreeVegetationLevelOfDetail : uint8
{
	LowImpostor,
	HighImpostor,
	Low,
	Medium,
	High,

	NumberOfTreeVegetationLevelOfDetails
};