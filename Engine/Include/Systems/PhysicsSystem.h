#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/CharacterMovement.h>
#include <Physics/ModelPhysicsSystem.h>
#include <Physics/PhysicsCore.h>
#include <Physics/RaycastConfiguration.h>
#include <Physics/RaycastResult.h>

class PhysicsSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(PhysicsSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE PhysicsSystem() NOEXCEPT
	{

	}

	/*
	*	Updates the physics system during the physics update phase.
	*/
	void PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the model physics system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD ModelPhysicsSystem *const RESTRICT GetModelPhysicsSystem() NOEXCEPT
	{
		return &_ModelPhysicsSystem;
	}

	/*
	*	Casts a ray.
	*/
	void CastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Registers a character movement.
	*/
	void RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT;

private:

	//The model physics system.
	ModelPhysicsSystem _ModelPhysicsSystem;

	//Container for all character movements.
	DynamicArray<CharacterMovement *RESTRICT> _CharacterMovements;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Updates one character movement.
	*/
	void UpdateCharacterMovement(const UpdateContext* const RESTRICT context, CharacterMovement* const RESTRICT movement) NOEXCEPT;

	/*
	*	Checks collision for character movement. Returns whether or not a collision occured.
	*/
	NO_DISCARD bool CheckCharacterMovementCollision(const CharacterMovement *const RESTRICT movement, Vector3<float32> *const RESTRICT collision_position) NOEXCEPT;

};