#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Physics.
#include <Physics/CharacterMovement.h>
#include <Physics/PhysicsCore.h>

class CharacterPhysicsSystem final
{

public:

	/*
	*	Updates the character physics system during the physics update phase.
	*/
	void PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Registers a character movement.
	*/
	void RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT;

private:

	//Container for all character movements.
	DynamicArray<CharacterMovement *RESTRICT> _CharacterMovements;

	/*
	*	Updates one character movement.
	*/
	void UpdateCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT;

	/*
	*	Simulates physics on one character movement.
	*/
	void SimulatePhysicsCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT;

	/*
	*	Checks on one character movement.
	*/
	NO_DISCARD bool CheckCollisionCharacterMovement(const CharacterMovement *const RESTRICT movement, Vector3<float32> *const RESTRICT collision_position) NOEXCEPT;

};