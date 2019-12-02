#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics constants.
namespace PhysicsConstants
{
	constexpr float GRAVITY{ 9.80'665f };
}

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	TERRAIN = BIT(0),

	ALL = TERRAIN
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);