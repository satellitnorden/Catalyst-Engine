#if defined(CATALYST_PHYSICS_JOLT)
//Header file.
#include <Physics/Native/CharacterController.h>

//Physics.
#include <Physics/Abstraction/JoltCharacterControllerAbstractionData.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Third party.
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

/*
*	Returns the world position.
*/
NO_DISCARD WorldPosition CharacterController::GetWorldPosition() NOEXCEPT
{
	//Retrieve the position.
	const JPH::Vec3 position{ _AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->GetPosition() };

	//Retrieve the shape.
	const JPH::CapsuleShape *const RESTRICT shape{ static_cast<const JPH::CapsuleShape *const RESTRICT>(_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->GetShape()) };

	//Return the position.
	return WorldPosition(Vector3<float32>(position.GetX(), position.GetY() - shape->GetHalfHeightOfCylinder(), position.GetZ()));
}

/*
*	Sets the world position.
*/
void CharacterController::SetWorldPosition(const WorldPosition &value) NOEXCEPT
{
	//Calculate the absolute position.
	const Vector3<float32> absolute_position{ value.GetAbsolutePosition() };

	//Retrieve the shape.
	const JPH::CapsuleShape *const RESTRICT shape{ static_cast<const JPH::CapsuleShape *const RESTRICT>(_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->GetShape()) };

	//Set the position.
	_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->SetPosition(JPH::Vec3(absolute_position._X, absolute_position._Y + shape->GetHalfHeightOfCylinder(), absolute_position._Z));
}

/*
*	Returns if the character is on ground.
*/
NO_DISCARD bool CharacterController::IsOnGround() NOEXCEPT
{
	return _AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->IsSupported();
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
	const JPH::Vec3 linear_velocity{ _AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->GetLinearVelocity() };

	return Vector3<float32>(linear_velocity.GetX(), linear_velocity.GetY(), linear_velocity.GetZ());
}

/*
*	Sets the linear velocity.
*/
void CharacterController::SetLinearVelocity(const Vector3<float32> &value) NOEXCEPT
{
	_AbstractionData.Get<JoltCharacterControllerAbstractionData>()->_Character->SetLinearVelocity(JPH::Vec3(value._X, value._Y, value._Z));
}
#endif