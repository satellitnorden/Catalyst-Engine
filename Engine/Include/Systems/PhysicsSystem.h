#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/CharacterMovement.h>
#include <Physics/PhysicsCore.h>
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
	*	Registers a character movement.
	*/
	void RegisterCharacterMovement(CharacterMovement *const RESTRICT movement) NOEXCEPT;

	/*
	*	Casts a ray.
	*/
	void CastRay(const Ray &ray, const PhysicsChannel channels, RaycastResult *const RESTRICT result) NOEXCEPT;

private:

	//Container for all character movements.
	DynamicArray<CharacterMovement *RESTRICT> _CharacterMovements;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT;

};