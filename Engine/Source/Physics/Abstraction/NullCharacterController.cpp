#if defined(CATALYST_PHYSICS_NULL)
//Header file.
#include <Physics/Native/CharacterController.h>

/*
*	Returns the world position.
*/
NO_DISCARD WorldPosition CharacterController::GetWorldPosition() NOEXCEPT
{
	//Return the world position.
	return WorldPosition();
}

/*
*	Sets the world position.
*/
void CharacterController::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	
}

/*
*	Returns if the character is on ground.
*/
NO_DISCARD bool CharacterController::IsOnGround() NOEXCEPT
{
	return false;
}

/*
*	Moves the character controller.
*/
void CharacterController::Move(const Vector3<float32> &displacement) NOEXCEPT
{
	
}

/*
*	Resizes the capsule height of this character controller.
*/
void CharacterController::ResizeCapsuleHeight(const float32 new_height) NOEXCEPT
{

}

/*
*	Returns the linear velocity.
*/
NO_DISCARD Vector3<float32> CharacterController::GetLinearVelocity() NOEXCEPT
{
	return Vector3<float32>(0.0f, 0.0f, 0.0f);
}

/*
*	Sets the linear velocity.
*/
void CharacterController::SetLinearVelocity(const Vector3<float32>& value) NOEXCEPT
{
	
}
#endif