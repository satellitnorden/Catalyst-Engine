#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	DynamicPhysical = BIT(0),
	Ocean = BIT(1),
	Terrain = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);