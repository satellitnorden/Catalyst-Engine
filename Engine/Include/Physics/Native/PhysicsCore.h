#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Enumeration.h>

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
CATALYST_ENUMERATION
(
	ModelCollisionType,
	uint8,
	NONE,
	BOX,
	COLLISION_MODEL
);