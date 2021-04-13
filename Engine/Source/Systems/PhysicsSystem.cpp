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
*	Terminates the physics for the given entity.
*/
void PhysicsSystem::TerminateEntityPhysics(Entity *const RESTRICT entity) NOEXCEPT
{
	//Terminate the sub-system physics for this entity.
	SubTerminateEntityPhysics(entity);
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
	//Set the default properties of the result.
	result->_HasHit = false;
	result->_HitDistance = configuration._MaximumHitDistance;
	result->_Type = RaycastResult::Type::NONE;

	//Raycast against dynamic models.
	if (TEST_BIT(configuration._PhysicsChannels, PhysicsChannel::DYNAMIC_MODELS))
	{
		_ModelPhysicsSystem.CastRayDynamicModels(ray, configuration, result);
	}

	//Raycast against static models.
	if (TEST_BIT(configuration._PhysicsChannels, PhysicsChannel::STATIC_MODELS))
	{
		_ModelPhysicsSystem.CastRayStaticModels(ray, configuration, result);
	}

	//Raycast against the terrain.
	if (TEST_BIT(configuration._PhysicsChannels, PhysicsChannel::TERRAIN))
	{
		CastRayTerrain(ray, configuration, result);
	}
}

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate() NOEXCEPT
{
	//Update the physics sub-system during the physics update phase.
	SubPhysicsUpdate();

	//Update the character physics system.
	_CharacterPhysicsSystem.PhysicsUpdate();
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