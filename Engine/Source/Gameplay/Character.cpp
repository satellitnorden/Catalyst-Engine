//Header file.
#include <Gameplay/Character.h>

//Systems.
#include <Systems/PhysicsSystem.h>

/*
*	Initialize this character.
*/
void Character::InitializeCharacter() NOEXCEPT
{
	//Create the character controller.
	{
		CharacterControllerConfiguration configuration;

		configuration._CapsuleRadius = 0.2'75f;
		configuration._CapsuleHeight = 2.0f;

		_CharacterController = PhysicsSystem::Instance->CreateCharacterController(configuration);
	}

	//Set the character controllers world position.
	_CharacterController->SetWorldPosition(WorldPosition(Vector3<float32>(0.0f, 1.0f, 0.0f)));

	//Set up the height spring damping system.
	_HeightSpringDampingSystem.SetCurrent(_CharacterConfiguration._StandingHeight);
	_HeightSpringDampingSystem.SetDesired(_CharacterConfiguration._StandingHeight);
	_HeightSpringDampingSystem.SetDampingCoefficient(32.0f);
	_HeightSpringDampingSystem.SetSpringConstant(256.0f);

	//Set up the speed spring damping system.
	_SpeedSpringDampingSystem.SetCurrent(_CharacterConfiguration._StandingWalkingSpeed);
	_SpeedSpringDampingSystem.SetDesired(_CharacterConfiguration._StandingWalkingSpeed);
	_SpeedSpringDampingSystem.SetDampingCoefficient(16.0f);
	_SpeedSpringDampingSystem.SetSpringConstant(128.0f);
}

/*
*	Updates this character.
*/
void Character::UpdateCharacter(const float32 delta_time) NOEXCEPT
{

}

/*
*	Post updates this character.
*/
void Character::PostUpdateCharacter(const float32 delta_time) NOEXCEPT
{
	//Update the speed spring damping system.
	float32 desired_speed;

	if (_IsCrouching)
	{
		desired_speed = _IsSprinting ? _CharacterConfiguration._CrouchingRunningSpeed : _CharacterConfiguration._CrouchingWalkingSpeed;
	}

	else
	{
		desired_speed = _IsSprinting ? _CharacterConfiguration._StandingRunningSpeed : _CharacterConfiguration._StandingWalkingSpeed;
	}

	_SpeedSpringDampingSystem.SetDesired(desired_speed);

	const float32 current_speed{ _SpeedSpringDampingSystem.Update(delta_time) };

	//Update the vertical velocity.
	if (_CharacterController->IsOnGround())
	{
		if (_CachedJump)
		{
			_VerticalVelocity = _CharacterConfiguration._JumpForce;
		}

		else
		{
			_VerticalVelocity = -PhysicsConstants::GRAVITY * delta_time;
		}
	}

	else
	{
		if (_CachedJump)
		{
			_VerticalVelocity = _CharacterConfiguration._JumpForce;
		}

		else
		_VerticalVelocity += -PhysicsConstants::GRAVITY * delta_time;
	}

	//Calculate the total displacement.
	Vector3<float32> total_displacement{ 0.0f, 0.0f, 0.0f };

	total_displacement += _CachedMovement * current_speed * delta_time;
	total_displacement += Vector3<float32>(0.0f, _VerticalVelocity, 0.0f) * delta_time;

	//Move the character controller.
	_CharacterController->Move(total_displacement);

	//Update the current height.
	_HeightSpringDampingSystem.SetDesired(_IsCrouching ? _CharacterConfiguration._CrouchingHeight : _CharacterConfiguration._StandingHeight);
	_CurrentHeight = _HeightSpringDampingSystem.Update(delta_time);
	_CharacterController->ResizeCapsuleHeight(_CurrentHeight);

	//Reset the cached movement/jump.
	_CachedMovement = Vector3<float32>(0.0f, 0.0f, 0.0f);
	_CachedJump = false;
}