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
#endif