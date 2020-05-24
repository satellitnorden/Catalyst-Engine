//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Terrain.
#include <Terrain/TerrainGeneralUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update the character physics system.
	_CharacterPhysicsSystem.PhysicsUpdate(context);
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