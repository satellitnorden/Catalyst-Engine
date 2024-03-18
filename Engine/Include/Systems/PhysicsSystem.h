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
#include <Physics/Native/ModelSimulationConfiguration.h>
#include <Physics/Native/PhysicsCore.h>
#include <Physics/Native/RaycastConfiguration.h>
#include <Physics/Native/RaycastResult.h>

//Rendering.
#include <Rendering/Native/Texture2D.h>

//World.
#include <World/Core/WorldSpaceAxisAlignedBoundingBox3D.h>
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
	*	Creates a height field actor.
	*/
	void CreateHeightFieldActor
	(
		const WorldPosition &world_position,
		const Texture2D<float32> &height_field,
		ActorHandle *const RESTRICT actor_handle
	) NOEXCEPT;

	/*
	*	Creates a model actor.
	*/
	void CreateModelActor
	(
		const WorldTransform &world_transform,
		const ModelCollisionType collision_type,
		const WorldSpaceAxisAlignedBoundingBox3D &world_space_axis_aligned_bounding_box,
		const CollisionModelHandle collision_model,
		const ModelSimulationConfiguration &simulation_configuration,
		ActorHandle *const RESTRICT actor_handle
	) NOEXCEPT;

	/*
	*	Destroys an actor.
	*/
	void DestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT;

	/*
	*	Returns the world transform for the given actor.
	*/
	void GetActorWorldTransform(const ActorHandle actor_handle, WorldTransform *const RESTRICT world_transform) NOEXCEPT;

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
	*	Creates a height field actor on the sub-system.
	*/
	void SubCreateHeightFieldActor
	(
		const WorldPosition &world_position,
		const Texture2D<float32> &height_field,
		ActorHandle *const RESTRICT actor_handle
	) NOEXCEPT;

	/*
	*	Creates a model actor on the sub-system.
	*/
	void SubCreateModelActor
	(
		const WorldTransform &world_transform,
		const ModelCollisionType collision_type,
		const WorldSpaceAxisAlignedBoundingBox3D &world_space_axis_aligned_bounding_box,
		const CollisionModelHandle collision_model,
		const ModelSimulationConfiguration &simulation_configuration,
		ActorHandle *const RESTRICT actor_handle
	) NOEXCEPT;

	/*
	*	Destroys an actor on the sub-system.
	*/
	void SubDestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT;

	/*
	*	Returns the world transform for the given actor on the sub-system.
	*/
	void SubGetActorWorldTransform(const ActorHandle actor_handle, WorldTransform* const RESTRICT world_transform) NOEXCEPT;

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