#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	Ocean = BIT(0),
	Terrain = BIT(1)
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);

//Enumeration covering all physics shapes.
enum class PhysicsShape : uint8
{
	Cube
};