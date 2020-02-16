#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics constants.
namespace PhysicsConstants
{
	constexpr float GRAVITY{ 9.80'665f };
}

//Enumeration covering all model collision types.
enum class ModelCollisionType : uint8
{
	NONE,
	AXIS_ALIGNED_BOUNDING_BOX
};

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	MODEL = BIT(0),
	TERRAIN = BIT(1),

	ALL = MODEL | TERRAIN
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);