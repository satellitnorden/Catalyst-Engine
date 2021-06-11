//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TerrainSystem.h>

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
	true);

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
*	Initializes the physics for the given entity.
*/
void PhysicsSystem::InitializeEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	//Initialize the sub-system physics for this entity.
	SubInitializeEntityPhysics(entity);
}

/*
*	Updates the world transform for the given entity.
*/
void PhysicsSystem::UpdateEntityWorldTransform(Entity *const RESTRICT entity, const WorldTransform &world_transform) NOEXCEPT
{
	//Update the world transform for this entity.
	SubUpdateEntityWorldTransform(entity, world_transform);
}

/*
*	Terminates the physics for the given entity.
*/
void PhysicsSystem::TerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	//Terminate the sub-system physics for this entity.
	SubTerminateEntityPhysics(entity);
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
}