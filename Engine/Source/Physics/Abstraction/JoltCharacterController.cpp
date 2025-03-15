#if defined(CATALYST_PHYSICS_JOLT)
//Header file.
#include <Physics/Native/CharacterController.h>

//Physics.
#include <Physics/Abstraction/JoltCharacterControllerAbstractionData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

/*
*	Returns the world position.
*/
NO_DISCARD WorldPosition CharacterController::GetWorldPosition() NOEXCEPT
{
	//Retrieve the position.
	const JPH::Vec3 position{ _AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->GetPosition() };

	//Return the position.
	return WorldPosition(Vector3<float32>(position.GetX(), position.GetY(), position.GetZ()));
}

/*
*	Sets the world position.
*/
void CharacterController::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	//Calculate the absolute position.
	const Vector3<float32> absolute_position{ value.GetAbsolutePosition() };

	//Set the position.
	_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->SetPosition(JPH::Vec3(absolute_position._X, absolute_position._Y, absolute_position._Z));
}

/*
*	Returns if the character is on ground.
*/
NO_DISCARD bool CharacterController::IsOnGround() NOEXCEPT
{
	return true;
}

/*
*	Moves the character controller.
*/
void CharacterController::Move(const Vector3<float32> &displacement) NOEXCEPT
{
#if 1
	_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->SetLinearVelocity(JPH::Vec3(displacement._X, displacement._Y, displacement._Z) / (1.0f / 60.0f));
#else
	//Retrieve the position.
	Vector3<float32> position{ GetWorldPosition().GetAbsolutePosition() };

	//Apply the displacement.
	position._X += displacement._X;
	position._Z += displacement._Z;

	//Set the position.
	SetWorldPosition(WorldPosition(position));
#endif
}

/*
*	Resizes the capsule height of this character controller.
*/
void CharacterController::ResizeCapsuleHeight(const float32 new_height) NOEXCEPT
{

}
#endif