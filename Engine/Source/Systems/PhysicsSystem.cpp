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
	//Update the model physics system.
	_ModelPhysicsSystem.PhysicsUpdate(context);

	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : _CharacterMovements)
	{
		UpdateCharacterMovement(context, movement);
	}
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

	//Raycast against models.
	if (TEST_BIT(configuration._PhysicsChannels, PhysicsChannel::MODEL))
	{
		_ModelPhysicsSystem.CastRayModels(ray, configuration, result);
	}

	//Raycast against the terrain.
	if (TEST_BIT(configuration._PhysicsChannels, PhysicsChannel::TERRAIN))
	{
		CastRayTerrain(ray, configuration, result);
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

/*
*	Updates one character movement.
*/
void PhysicsSystem::UpdateCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Apply the jump input.
	if (!movement->_IsJumping && movement->_JumpInput > 0.0f)
	{
		//Apply the movement input to the velocity.
		movement->_Velocity += movement->_MovementInput;
		movement->_Velocity += VectorConstants::UP * movement->_JumpInput;

		movement->_IsJumping = true;
	}


	//Apply different logic based on whether or not the character is jumping or not.
	if (movement->_IsJumping)
	{
		//Apply gravity.
		movement->_Velocity._Y -= PhysicsConstants::GRAVITY * context->_DeltaTime;

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;
	}

	else
	{
		//Apply gravity.
		movement->_Velocity._Y -= PhysicsConstants::GRAVITY * context->_DeltaTime;

		//Apply the velocity to the position.
		movement->_Position += movement->_Velocity * context->_DeltaTime;

		//Apply the movement input to the position.
		movement->_Position += movement->_MovementInput * context->_DeltaTime;

		//Apply friction to the X and Z axes of the velocity.
		movement->_Velocity._X *= 0.1f;
		movement->_Velocity._Z *= 0.1f;
	}

	//Reset the inputs.
	movement->_MovementInput = VectorConstants::ZERO;
	movement->_JumpInput = 0.0f;

	//Check collision.
	Vector3<float32> collision_position;

	if (CheckCharacterMovementCollision(movement, &collision_position))
	{
		//Resolve collision.
		movement->_Position = collision_position;
		movement->_Velocity = VectorConstants::ZERO;
		movement->_IsJumping = false;
	}
}

/*
*	Checks collision for character movement. Returns whether or not a collision occured.
*/
NO_DISCARD bool PhysicsSystem::CheckCharacterMovementCollision(const CharacterMovement *const RESTRICT movement, Vector3<float32> *const RESTRICT collision_position) NOEXCEPT
{
	//Check collision against the terrain.
	float32 terrain_height;

	if (TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height))
	{
		if (movement->_Position._Y <= terrain_height)
		{
			//Collision was found!
			*collision_position = Vector3<float32>(movement->_Position._X, terrain_height, movement->_Position._Z);

			return true;
		}
	}

	/*
	//Check collision against models.
	for (const Pair<uint64, ModelCollisionData>& data : _ModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				const AxisAlignedBoundingBox &box{ data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox };

				if (box.IsInside(movement->_Position))
				{
					//Collision was found!
					*collision_position = Vector3<float32>(movement->_Position._X, box._Maximum._Y, movement->_Position._Z);

					return true;
				}

				break;
			}
		}
	}
	*/

	//No collision was found!
	return false;
}