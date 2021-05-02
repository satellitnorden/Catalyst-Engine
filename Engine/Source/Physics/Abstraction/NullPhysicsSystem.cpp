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
*	Initializes the sub-system physics for the given entity.
*/
void PhysicsSystem::SubInitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	
}

/*
*	Updates the sub-system world transform for the given entity.
*/
void PhysicsSystem::SubUpdateEntityWorldTransform(Entity *const RESTRICT entity, const WorldTransform &world_transform) NOEXCEPT
{
	
}

/*
*	Terminates the sub-system physics for the given entity.
*/
void PhysicsSystem::SubTerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
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
void PhysicsSystem::SubAddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT
{
	
}

/*
*	Creates a sub-system character controller.
*/
RESTRICTED NO_DISCARD CharacterController *const RESTRICT PhysicsSystem::SubCreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT
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