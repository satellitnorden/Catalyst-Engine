#if defined(CATALYST_PHYSICS_PHYSX)
//Header file.
#include <Physics/Native/CharacterController.h>

//Physics.
#include <Physics/Abstraction/PhysXCharacterControllerAbstractionData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Returns the world position.
*/
NO_DISCARD WorldPosition CharacterController::GetWorldPosition() NOEXCEPT
{
	//Cast the abstraction data.
	PhysXCharacterControllerAbstractionData *const RESTRICT abstraction_data{ _AbstractionData.Get<PhysXCharacterControllerAbstractionData>() };

	//Construct the world position.
	WorldPosition world_position;

	{
		const physx::PxExtendedVec3 &position{ abstraction_data->_Controller->getFootPosition() };
		world_position.SetLocalPosition(Vector3<float32>(static_cast<float32>(position.x), static_cast<float32>(position.y), static_cast<float32>(-position.z)));
	}

	//Return the world position.
	return world_position;
}

/*
*	Sets the world position.
*/
void CharacterController::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	//Cast the abstraction data.
	PhysXCharacterControllerAbstractionData *const RESTRICT abstraction_data{ _AbstractionData.Get<PhysXCharacterControllerAbstractionData>() };

	//Set the foot position.
	const Vector3<float32> absolute_world_position{ value.GetAbsolutePosition() };
	const physx::PxExtendedVec3 physx_world_position{ static_cast<float64>(absolute_world_position._X), static_cast<float64>(absolute_world_position._Y), static_cast<float64>(-absolute_world_position._Z) };

	abstraction_data->_Controller->setFootPosition(physx_world_position);
}

/*
*	Returns if the character is on ground.
*/
NO_DISCARD bool CharacterController::IsOnGround() NOEXCEPT
{
	//Cast the abstraction data.
	PhysXCharacterControllerAbstractionData *const RESTRICT abstraction_data{ _AbstractionData.Get<PhysXCharacterControllerAbstractionData>() };

	//Return if the character is on ground.
	return abstraction_data->_IsOnGround;
}

/*
*	Moves the character controller.
*/
void CharacterController::Move(const Vector3<float32> &displacement) NOEXCEPT
{
	//Define constants.
	constexpr float32 MINIMUM_DISTANCE{ 0.00f };
	constexpr float32 MINIMUM_DISTANCE_SQUARED{ MINIMUM_DISTANCE * MINIMUM_DISTANCE };

	//Cast the abstraction data.
	PhysXCharacterControllerAbstractionData *const RESTRICT abstraction_data{ _AbstractionData.Get<PhysXCharacterControllerAbstractionData>() };

	//Calculate the physx displacement.
	const physx::PxVec3 physx_displacement{ physx::PxVec3(displacement._X, displacement._Y, -displacement._Z) };

	//Cache the delta time.
	const float32 delta_time{ CatalystEngineSystem::Instance->GetDeltaTime() };

	//Set up the filters.
	const physx::PxControllerFilters filters;

	//Move the controller.
	const physx::PxControllerCollisionFlags collision_flags{ abstraction_data->_Controller->move(physx_displacement, MINIMUM_DISTANCE, 1.0f, filters) };

	//Remember if the character is on ground.
	//if (Vector3<float32>::LengthSquared(displacement) >= MINIMUM_DISTANCE_SQUARED)
	{
		if (TEST_BIT(collision_flags, physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
		{
			abstraction_data->_IsOnGround = true;
		}

		else
		{
			abstraction_data->_IsOnGround = false;
		}
	}
}

/*
*	Resizes the capsule height of this character controller.
*/
void CharacterController::ResizeCapsuleHeight(const float32 new_height) NOEXCEPT
{
	//Cast the abstraction data.
	PhysXCharacterControllerAbstractionData *const RESTRICT abstraction_data{ _AbstractionData.Get<PhysXCharacterControllerAbstractionData>() };

	//Resize!
	abstraction_data->_Controller->resize(new_height);
}
#endif