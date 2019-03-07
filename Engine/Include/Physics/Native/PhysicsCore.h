#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	DynamicPhysical = BIT(0),
	Ocean = BIT(1),
	Terrain = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);

//Enumeration covering all physics shapes.
enum class PhysicsShape : uint8
{
	Cube
};