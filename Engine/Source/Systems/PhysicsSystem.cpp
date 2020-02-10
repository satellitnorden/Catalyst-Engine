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
		UpdateCharacterMovement(context, movement);
	}
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
	if (TEST_BIT(channels, PhysicsChannel::TERRAIN))
	{
		CastRayTerrain(ray, result);
	}
}

/*
*	Registers a character movement.
*/
void PhysicsSystem::RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	_CharacterMovements.Emplace(movement);
}

/*
*	Casts a ray against the terrain.
*/
void PhysicsSystem::CastRayTerrain(const Ray &ray, RaycastResult *const RESTRICT result) NOEXCEPT
{
	/*
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
	*/
}

/*
*	Updates one character movement.
*/
void PhysicsSystem::UpdateCharacterMovement(const UpdateContext* const RESTRICT context, CharacterMovement* const RESTRICT movement) NOEXCEPT
{
	//Apply different logic based on whether or not the character is jumping or not.
	if (movement->_IsJumping)
	{
		//Apply gravity.
		movement->_Velocity._Y -= PhysicsConstants::GRAVITY * context->_DeltaTime;

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;

		//Clamp the position to the terrain height.
		float terrain_height{ 0.0f };

		TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height);

		if (movement->_Position._Y <= terrain_height)
		{
			movement->_Position._Y = terrain_height;

			//Signal that this character is no longer jumping.
			movement->_IsJumping = false;
		}
	}

	else
	{
		//Apply the movement input to the velocity.
		movement->_Velocity += movement->_MovementInput;

		//Apply the jump input to the velocity.
		if (movement->_JumpInput > 0.0f)
		{
			movement->_Velocity += VectorConstants::UP * movement->_JumpInput;

			movement->_IsJumping = true;
		}

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;

		//Clamp the position to the terrain height.
		if (!movement->_IsJumping)
		{
			float terrain_height{ 0.0f };

			TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height);

			movement->_Position._Y = terrain_height;

			//Apply some ground damping when walking on terrain.
			movement->_Velocity *= 0.01f;
		}
	}

	//Reset the inputs.
	movement->_MovementInput = VectorConstants::ZERO;
	movement->_JumpInput = 0.0f;
}