#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Vector3.h>

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	DynamicPhysical = BIT(0),
	Ocean = BIT(1),
	Terrain = BIT(2)
};

//Enumeration covering all physics shapes.
enum class PhysicsShape : uint8
{
	Sphere
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);