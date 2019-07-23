//Header file.
#include <Systems/PhysicsSystem.h>

//Math.
#include <Math/Core/CatalystGeometryMath.h>

//Systems.
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(PhysicsSystem);

/*
*	Updates the physics system during the physics update phase.
*/
void PhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : _CharacterMovements)
	{
		//Apply the movement.
		movement->_Position += movement->_Velocity * context->_DeltaTime;
		movement->_Velocity = VectorConstants::ZERO;

		//Clamp the position to the terrain height.
		TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &movement->_Position._Y);
	}
}

/*
*	Registers a character movement.
*/
void PhysicsSystem::RegisterCharacterMovement(CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	_CharacterMovements.EmplaceSlow(movement);
}

/*
*	Casts a ray.
*/
void PhysicsSystem::CastRay(const Ray &ray, const PhysicsChannel channels, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Set the default properties of the result.
	result->_HasHit = false;
	result->_HitDistance = FLOAT_MAXIMUM;
	result->_HitEntity = nullptr;

	//Raycast against the terrain.
	if (TEST_BIT(channels, PhysicsChannel::Terrain))
	{
		CastRayTerrain(ray, result);
	}

	//Raycast against models.
	if (TEST_BIT(channels, PhysicsChannel::Model))
	{

	}
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT
{
	//Ray march until the ray has hit the terrain.
	const uint32 number_of_steps{ static_cast<uint32>(ray._MaximumHitDistance) + 1 };

	for (uint32 i{ 0 }; i < number_of_steps; ++i)
	{
		//Calculate the current ray position.
		const Vector3<float> current_ray_position{ ray._Origin + ray._Direction * static_cast<float>(i) };

		//Get the terrain height at the current ray position.
		float terrain_height;
		
		if (!TerrainSystem::Instance->GetTerrainHeightAtPosition(current_ray_position, &terrain_height))
		{
			//The ray is outside the bounds of the terrain, abort.
			return;
		}

		//If the current ray position's height is lower than the terrain height at this position, there is a hit.
		if (current_ray_position._Y <= terrain_height)
		{
			result->_HasHit = true;
			result->_HitDistance = static_cast<float>(i);
			result->_HitEntity = nullptr;

			return;
		}
	}
}