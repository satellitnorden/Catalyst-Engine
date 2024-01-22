//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Initializes the physics system.
*/
void PhysicsSystem::Initialize() NOEXCEPT
{
	//Only one thread can try to initialize the physics system at one.
	_InitializationLock.Lock();

	//Only initialize once.
	if (_Initialized)
	{
		//Other threads are now free to try to initialize the physics system. (:
		_InitializationLock.Unlock();

		return;
	}

	//Initialize the sub system.
	SubInitialize();

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void *const RESTRICT arguments)
	{
		static_cast<PhysicsSystem *const RESTRICT>(arguments)->PhysicsUpdate();
	},
	this,
	UpdatePhase::PHYSICS,
	UpdatePhase::RENDER,
	false,
	false);

	//The physics system is now initialized!
	_Initialized = true;

	//Other threads are now free to try to initialize the physics system. (:
	_InitializationLock.Unlock();
}

/*
*	Terminates the physics system.
*/
void PhysicsSystem::Terminate() NOEXCEPT
{
	//Terminate the sub system.
	SubTerminate();
}

/*
*	Creates a collision model.
*/
void PhysicsSystem::CreateCollisionModel(const CollisionModelData &collision_model_data, CollisionModelHandle *const RESTRICT collision_model) NOEXCEPT
{
	//Create the sollision model on the sub-system.
	SubCreateCollisionModel(collision_model_data, collision_model);
}

/*
*	Creates a height field actor.
*/
void PhysicsSystem::CreateHeightFieldActor
(
	const WorldPosition &world_position,
	const Texture2D<float32> &height_field,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	//Create the height field on the sub-system.
	SubCreateHeightFieldActor(world_position, height_field, actor_handle);
}

/*
*	Creates a model actor.
*/
void PhysicsSystem::CreateModelActor
(
	const WorldTransform &world_transform,
	const ModelCollisionConfiguration &collision_configuration,
	const WorldSpaceAxisAlignedBoundingBox3D &world_space_axis_aligned_bounding_box,
	const CollisionModelHandle collision_model,
	const ModelSimulationConfiguration &simulation_configuration,
	ActorHandle *const RESTRICT actor_handle
) NOEXCEPT
{
	//Create the model actor on the sub-system.
	SubCreateModelActor
	(
		world_transform,
		collision_configuration,
		world_space_axis_aligned_bounding_box,
		collision_model,
		simulation_configuration,
		actor_handle
	);
}

/*
*	Destroys an actor.
*/
void PhysicsSystem::DestroyActor(ActorHandle *const RESTRICT actor_handle) NOEXCEPT
{
	//Destroy the actor on the sub-system.
	SubDestroyActor(actor_handle);
}

/*
*	Returns the world transform for the given actor.
*/
void PhysicsSystem::GetActorWorldTransform(const ActorHandle actor_handle, WorldTransform* const RESTRICT world_transform) NOEXCEPT
{
	//Retrieve the world transform from the sub-system.
	SubGetActorWorldTransform(actor_handle, world_transform);
}

/*
*	Creates a character controller.
*/
RESTRICTED NO_DISCARD CharacterController *const RESTRICT PhysicsSystem::CreateCharacterController(const CharacterControllerConfiguration &configuration) NOEXCEPT
{
	//Create the sub-system character controller.
	return SubCreateCharacterController(configuration);
}

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Cast a sub-system ray.
	SubCastRay(ray, configuration, result);
}

/*
*	Adds an impulse at the given world position with the given force.
*/
void PhysicsSystem::AddImpulse(const WorldPosition &world_position, const float32 force) NOEXCEPT
{
	//Add a sub-system impulse.
	SubAddImpulse(world_position, force);
}

/*
*	Builds a collision model.
*/
void PhysicsSystem::BuildCollisionModel(const ModelFile &model_file, CollisionModelData *const RESTRICT collision_model_data) NOEXCEPT
{
	//Build the collision model on the sub-system.
	SubBuildCollisionModel(model_file, collision_model_data);
}

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate() NOEXCEPT
{
	//Update the physics sub-system during the physics update phase.
	SubPhysicsUpdate();
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	/*
	//Ray march the terrain until there is a hit.
	float current_step{ 0.0f };

	while (current_step < result->_HitDistance)
	{
		//Calculate the sample position.
		const Vector3<float> sample_position{ ray._Origin + ray._Direction * current_step };

		//Retrieve the sample height.
		float sample_height;

		if (TerrainSystem::Instance->GetTerrainHeightAtPosition(sample_position, &sample_height))
		{
			//If the sample height is higher than the sample positions height, there was a hit!
			if (sample_height >= sample_position._Y)
			{
				result->_HasHit = true;
				result->_HitDistance = current_step;
				result->_Type = RaycastResult::Type::TERRAIN;

				return;
			}
		}

		//Advance the current step.
		current_step += configuration._TerrainRayMarchStep;
	}
	*/
}