#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Spinlock.h>

//File.
#include <File/Types/ModelFile.h>

//Math.
#include <Math/Geometry/Ray.h>

//Physics.
#include <Physics/Native/CharacterController.h>
#include <Physics/Native/CharacterControllerConfiguration.h>
#include <Physics/Native/CollisionModelData.h>
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/RaycastConfiguration.h>
#include <Physics/Native/RaycastResult.h>

//World.
#include <World/Core/WorldTransform.h>

class ALIGN(8) PhysicsSystem final
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
	*	Terminates the physics system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Creates a collision model.
	*/
	void CreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_model) NOEXCEPT;

	/*
	*	Initializes the physics for the given entity.
	*/
	void InitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Updates the world transform for the given entity.
	*/
	void UpdateEntityWorldTransform(Entity *const RESTRICT entity, const WorldTransform &world_transform) NOEXCEPT;

	/*
	*	Terminates the physics for the given entity.
	*/
	void TerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Creates a character controller.
	*/
	RESTRICTED NO_DISCARD CharacterController *const RESTRICT CreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT;

	/*
	*	Casts a ray.
	*/
	void CastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Adds an impulse at the given world position with the given force.
	*/
	void AddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT;

	/*
	*	Builds a collision model.
	*/
	void BuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT;

private:

	//The initialization lock.
	Spinlock _InitializationLock;

	//Denotes if the physics system is initialized.
	bool _Initialized{ false };

	/*
	*	Updates the physics system during the physics update phase.
	*/
	void PhysicsUpdate() NOEXCEPT;

	/*
	*	Initializes the physics sub-system.
	*/
	void SubInitialize() NOEXCEPT;

	/*
	*	Creates a collision model on the sub-system.
	*/
	void SubCreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_data) NOEXCEPT;

	/*
	*	Updates the physics sub-system during the physics update phase.
	*/
	void SubPhysicsUpdate() NOEXCEPT;

	/*
	*	Terminates the physics sub-system.
	*/
	void SubTerminate() NOEXCEPT;

	/*
	*	Initializes the sub-system physics for the given entity.
	*/
	void SubInitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Updates the sub-system world transform for the given entity.
	*/
	void SubUpdateEntityWorldTransform(Entity *const RESTRICT entity, const WorldTransform &world_transform) NOEXCEPT;

	/*
	*	Terminates the sub-system physics for the given entity.
	*/
	void SubTerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT;

	/*
	*	Casts a sub-system ray.
	*/
	void SubCastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

	/*
	*	Adds an sub-system impulse at the given world position with the given force.
	*/
	void SubAddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT;

	/*
	*	Creates a sub-system character controller.
	*/
	RESTRICTED NO_DISCARD CharacterController *const RESTRICT SubCreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT;

	/*
	*	Builds a collision model on the sub-system.
	*/
	void SubBuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT;

	/*
	*	Casts a ray against the terrain.
	*/
	void CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT;

};