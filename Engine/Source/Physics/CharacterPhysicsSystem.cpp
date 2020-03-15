//Header file.
#include <Physics/CharacterPhysicsSystem.h>

//Systems.
#include <Systems/PhysicsSystem.h>
#include <Systems/TerrainSystem.h>

/*
*	Updates the character physics system during the physics update phase.
*/
void CharacterPhysicsSystem::PhysicsUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Update all character movements.
	for (CharacterMovement *const RESTRICT movement : _CharacterMovements)
	{
		UpdateCharacterMovement(context, movement);
	}
}

/*
*	Registers a character movement.
*/
void CharacterPhysicsSystem::RegisterCharacterMovement(CharacterMovement* const RESTRICT movement) NOEXCEPT
{
	//Add it to the container.
	_CharacterMovements.Emplace(movement);
}

/*
*	Updates one character movement.
*/
void CharacterPhysicsSystem::UpdateCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT
{
	//Simulate physics on the character movement.
	SimulatePhysicsCharacterMovement(context, movement);

	//Check collision.
	Vector3<float32> collision_position;

	while (CheckCollisionCharacterMovement(movement, &collision_position))
	{
		//Resolve collision.
		movement->_Position = collision_position;
		movement->_Velocity = VectorConstants::ZERO;
		movement->_IsJumping = false;
	}
}

/*
*	Simulates physics on one character movement.
*/
void CharacterPhysicsSystem::SimulatePhysicsCharacterMovement(const UpdateContext *const RESTRICT context, CharacterMovement *const RESTRICT movement) NOEXCEPT
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
}

/*
*	Checks on one character movement.
*/
NO_DISCARD bool CharacterPhysicsSystem::CheckCollisionCharacterMovement(const CharacterMovement *const RESTRICT movement, Vector3<float32> *const RESTRICT collision_position) NOEXCEPT
{
	//Check collision against the terrain.
	float32 terrain_height;

	if (TerrainSystem::Instance->GetTerrainHeightAtPosition(movement->_Position, &terrain_height))
	{
		if (movement->_Position._Y < terrain_height)
		{
			//Collision was found!
			*collision_position = Vector3<float32>(movement->_Position._X, terrain_height, movement->_Position._Z);

			return true;
		}
	}

	//Check collision against static models.
	for (const Pair<uint64, ModelCollisionData> &data : PhysicsSystem::Instance->GetModelPhysicsSystem()->GetStaticModelCollisionData())
	{
		switch (data._Second._Type)
		{
			case ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX:
			{
				if (data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox.IsInside(movement->_Position))
				{
					//Collision was found!
					*collision_position = Vector3<float32>(movement->_Position._X, data._Second._AxisAlignedBoundingBoxData._AxisAlignedBoundingBox._Maximum._Y, movement->_Position._Z);

					return true;
				}

				break;
			}
		}
	}

	//No collision was found!
	return false;
}