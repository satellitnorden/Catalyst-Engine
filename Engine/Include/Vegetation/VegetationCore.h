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
	Tree,

	NumberOfVegetationTypes
};