#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/Native/CharacterPhysicsSystem.h>
#include <Physics/Native/ModelPhysicsSystem.h>
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/RaycastConfiguration.h>
#include <Physics/Native/RaycastResult.h>

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
	*	Initializes the physics system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Initializes the physics for the given entity.
	*/
	void InitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates the physics for the given entity.
	*/
	void TerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates the physics system.
	*/
	void Terminate() NOEXCEPT;

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
	*	Updates the physics system during the physics update phase.
	*/
	void PhysicsUpdate() NOEXCEPT;

	/*
	*	Initializes the physics sub-system.
	*/
	void SubInitialize() NOEXCEPT;

	/*
	*	Updates the physics sub-system during the physics update phase.
	*/
	void SubPhysicsUpdate() NOEXCEPT;

	/*
	*	Initializes the sub-system physics for the given entity.
	*/
	void SubInitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates the sub-system physics for the given entity.
	*/
	void SubTerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Terminates the physics sub-system.
	*/
	void SubTerminate() NOEXCEPT;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

};