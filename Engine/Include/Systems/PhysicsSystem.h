#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/CharacterMovement.h>
#include <Physics/PhysicsCore.h>
#include <Physics/RayCastResult.h>

class PhysicsSystem final
{

public:

	/*
	*	Updates the physics system during the physics update phase.
	*/
	static void PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers a character movement.
	*/
	static void RegisterCharacterMovement(CharacterMovement *const RESTRICT movement) NOEXCEPT;

};