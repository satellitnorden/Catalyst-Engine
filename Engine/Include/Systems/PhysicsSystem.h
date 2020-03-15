#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/CharacterPhysicsSystem.h>
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
	*	Returns the character physics system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD CharacterPhysicsSystem *const RESTRICT GetCharacterPhysicsSystem() NOEXCEPT
	{
		return &_CharacterPhysicsSystem;
	}

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

private:

	//The character physics system.
	CharacterPhysicsSystem _CharacterPhysicsSystem;

	//The model physics system.
	ModelPhysicsSystem _ModelPhysicsSystem;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

};