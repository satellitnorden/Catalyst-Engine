#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	Terrain = BIT(0),
	Model = BIT(1),

	All = Terrain | Model
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);