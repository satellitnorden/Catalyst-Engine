#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/Map.h>
#include <Core/General/UpdateContext.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/CharacterMovement.h>
#include <Physics/ModelCollisionData.h>
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
	*	Casts a ray.
	*/
	void CastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Registers a character movement.
	*/
	void RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT;

	/*
	*	Registers model collision data.
	*/
	void RegisterModelCollisionData(const uint64 entity_identifier, const ModelCollisionData &data) NOEXCEPT;

	/*
	*	Unregisters model collision data.
	*/
	void UnregisterModelCollisionData(const uint64 entity_identifier) NOEXCEPT;

private:

	//Container for all character movements.
	DynamicArray<CharacterMovement *RESTRICT> _CharacterMovements;

	//Container for all model collision data.
	Map<uint64, ModelCollisionData> _ModelCollisionData;

	/*
	*	Casts a ray against models.
	*/
	void CastRayModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Updates one character movement.
	*/
	void UpdateCharacterMovement(const UpdateContext* const RESTRICT context, CharacterMovement* const RESTRICT movement) NOEXCEPT;

};