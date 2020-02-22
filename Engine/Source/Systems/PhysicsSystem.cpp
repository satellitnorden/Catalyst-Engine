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
		CastRayModels(ray, configuration, result);
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
*	Registers model collision data.
*/
void PhysicsSystem::RegisterModelCollisionData(const uint64 entity_identifier, const ModelCollisionData& data) NOEXCEPT
{
	_ModelCollisionData[entity_identifier] = data;
}

/*
*	Unregisters model collision data.
*/
void PhysicsSystem::UnregisterModelCollisionData(const uint64 entity_identifer) NOEXCEPT
{
	
}

/*
*	Casts a ray against models.
*/
void PhysicsSystem::CastRayModels(const Ray &ray, const RaycastConfiguration &configuration, RaycastResult *const RESTRICT result) NOEXCEPT
{
	for (const Pair<uint64, ModelCollisionData>& data : _ModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				float intersection_distance{ FLOAT_MAXIMUM };

				if (CatalystGeometryMath::RayBoxIntersection(ray, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox, &intersection_distance)
					&& result->_HitDistance > intersection_distance)
				{
					result->_HasHit = true;
					result->_HitDistance = intersection_distance;
					result->_Type = RaycastResult::Type::MODEL;
				}

				break;
			}
		}
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

/*
*	Updates one character movement.
*/
void PhysicsSystem::UpdateCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Cache the character capsule.
	Capsule capsule{ movement->_Position, movement->_Position + Vector3<float>(0.0f, movement->_Height, 0.0f), movement->_Radius };

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

			//Reset the velocity.
			movement->_Velocity = VectorConstants::ZERO;
		}
	}

	//Reset the inputs.
	movement->_MovementInput = VectorConstants::ZERO;
	movement->_JumpInput = 0.0f;

	/*
	//Resolve collisions.
	for (const Pair<uint64, ModelCollisionData>& data : _ModelCollisionData)
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				if (data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox.IsInsideXZ(movement->_Position))
				{
					movement->_Position._Y = CatalystBaseMath::Maximum<float>(movement->_Position._Y, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox._Maximum._Y);
				}

				break;
			}
		}
	}
	*/
}