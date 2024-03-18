#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Generated.
#include <Generated/Enumeration.Generated.h>

//Physics constants.
namespace PhysicsConstants
{
	constexpr float32 GRAVITY{ 9.80'665f };
}

/*
*	Type aliases.
*/
using ActorHandle = void *RESTRICT;
using CollisionModelHandle = void *RESTRICT;

//Enumeration covering all model collision types.
/*!CATALYST_ENUMERATION
enum class ModelCollisionType : uint8
{
	NONE,
	BOX,
	COLLISION_MODEL
};
*/

//Enumeration covering all physics channels.
enum class PhysicsChannel : uint8
{
	CHARACTERS = BIT(0),
	DYNAMIC_MODELS = BIT(1),
	STATIC_MODELS = BIT(2),
	TERRAIN = BIT(3),

	ALL = CHARACTERS | DYNAMIC_MODELS | STATIC_MODELS | TERRAIN
};

ENUMERATION_BIT_OPERATIONS(PhysicsChannel);