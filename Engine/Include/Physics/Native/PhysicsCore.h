#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics constants.
namespace PhysicsConstants
{
	constexpr float32 GRAVITY{ 9.80'665f };
}

/*
*	Type aliases.
*/
using CollisionModelHandle = void *RESTRICT;

//Enumeration covering all model collision types.
enum class ModelCollisionType : uint8
{
	NONE,
	BOX,
	COLLISION_MODEL
};

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	DYNAMIC_MODELS = BIT(0),
	STATIC_MODELS = BIT(1),
	TERRAIN = BIT(2),

	ALL = DYNAMIC_MODELS | STATIC_MODELS | TERRAIN
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);