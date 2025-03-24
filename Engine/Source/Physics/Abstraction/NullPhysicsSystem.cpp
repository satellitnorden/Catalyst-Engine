#if defined(CATALYST_PHYSICS_NULL)
//Header file.
#include <Systems/PhysicsSystem.h>

/*
*	Initializes the physics sub-system.
*/
void PhysicsSystem::SubInitialize() NOEXCEPT
{
	
}

/*
*	Creates a collision model on the sub-system.
*/
void PhysicsSystem::SubCreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_model) NOEXCEPT
{
	
}

/*
*	Updates the physics sub-system during the physics update phase.
*/
void PhysicsSystem::SubPhysicsUpdate() NOEXCEPT
{
	
}

/*
*	Terminates the physics sub-system.
*/
void PhysicsSystem::SubTerminate() NOEXCEPT
{
	
}

/*
	*	Creates a height field actor on the sub-system.
	*/
void PhysicsSystem::SubCreateHeightFieldActor
(
	const WorldPosition &world_position,
	const Texture2D<float32> &height_field,
	const bool add_to_world,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{

}

/*
*	Creates a model actor on the sub-system.
*/
void PhysicsSystem::SubCreateModelActor
(
	Entity *const RESTRICT entity,
	const WorldTransform &world_transform,
	const ModelCollisionType collision_type,
	const AxisAlignedBoundingBox3D &axis_aligned_bounding_box,
	const CollisionModelHandle collision_model,
	const ModelSimulationConfiguration &simulation_configuration,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{

}

/*
*	Adds the given actor to the world on the sub-system.
*/
void PhysicsSystem::SubAddToWorld(const ActorHandle actor_handle) NOEXCEPT
{

}

/*
*	Destroys an actor on the sub-system.
*/
void PhysicsSystem::SubDestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{

}

/*
*	Returns the world transform for the given actor on the sub-system.
*/
void PhysicsSystem::SubGetActorWorldTransform(const ActorHandle actor_handle, WorldTransform *const RESTRICT world_transform) NOEXCEPT
{

}

/*
*	Updates the world transform of the given actor on the sub-system.
*/
void PhysicsSystem::SubUpdateWorldTransform(const WorldTransform &world_transform, ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{

}

/*
*	Casts a sub-system ray.
*/
void PhysicsSystem::SubCastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	
}

/*
*	Adds an sub-system impulse at the given world position with the given force.
*/
void PhysicsSystem::SubAddImpulse(const WorldPosition &world_position, const float32 radius, const float32 force) NOEXCEPT
{
	
}

/*
*	Creates a sub-system character controller.
*/
RESTRICTED NO_DISCARD CharacterController *const RESTRICT PhysicsSystem::SubCreateCharacterController(Entity *const RESTRICT entity, const CharacterControllerConfiguration &configuration) NOEXCEPT
{
	return nullptr;
}

/*
*	Builds a collision model on the sub-system.
*/
void PhysicsSystem::SubBuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT
{
	
}
#endif