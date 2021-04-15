//Header file.
#include <Gameplay/Player.h>

//Core.
#include <Core/General/Perceiver.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>

/*
*	Initialize this player.
*/
void Player::InitializePlayer() NOEXCEPT
{
	//Initialize this character.
	InitializeCharacter();

	//Set up the input state.
	for (uint8 i{ 0 }; i < 2; ++i)
	{
		_CurrentInputState._MovementSpringDampingSystems[i].SetCurrent(0.0f);
		_CurrentInputState._MovementSpringDampingSystems[i].SetDesired(0.0f);
		_CurrentInputState._MovementSpringDampingSystems[i].SetDampingCoefficient(32.0f);
		_CurrentInputState._MovementSpringDampingSystems[i].SetSpringConstant(512.0f);
	}

	//Set up the crouch spring damping system.
	_CrouchSpringDampingSystem.SetCurrent(2.0f);
	_CrouchSpringDampingSystem.SetDesired(2.0f);
	_CrouchSpringDampingSystem.SetDampingCoefficient(32.0f);
	_CrouchSpringDampingSystem.SetSpringConstant(256.0f);

	//Set up the speed spring damping system.
	_SpeedSpringDampingSystem.SetCurrent(_CharacterConfiguration._StandingWalkingSpeed);
	_SpeedSpringDampingSystem.SetDesired(_CharacterConfiguration._StandingWalkingSpeed);
	_SpeedSpringDampingSystem.SetDampingCoefficient(16.0f);
	_SpeedSpringDampingSystem.SetSpringConstant(128.0f);
}

/*
*	Updates this player.
*/
void Player::UpdatePlayer(const float32 delta_time) NOEXCEPT
{
	//Update this character.
	UpdateCharacter(delta_time);

	//Update the current input state.
	UpdateCurrentInputState(delta_time);

	//Apply rotation.
	_Rotation += Vector3<float32>(_CurrentInputState._Rotation._X * delta_time, _CurrentInputState._Rotation._Y * delta_time, 0.0f);
	_Rotation._X = CatalystBaseMath::Clamp<float32>(_Rotation._X, CatalystBaseMath::DegreesToRadians(-89.0f), CatalystBaseMath::DegreesToRadians(89.0f));

	//Calculate the forward and right vector.
	Vector3<float32> forward{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation) };
	Vector3<float32> right{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_Rotation) };

	//Ignore the Y axis on the forward and right vectors.
	forward.NormalizeXZ();
	right.NormalizeXZ();

	//Determine the desired speed.
	float32 desired_speed;

	if (_CurrentInputState._IsCrouching)
	{
		desired_speed = _CurrentInputState._IsRunning ? _CharacterConfiguration._CrouchingRunningSpeed : _CharacterConfiguration._CrouchingWalkingSpeed;
	}

	else
	{
		desired_speed = _CurrentInputState._IsRunning ? _CharacterConfiguration._StandingRunningSpeed : _CharacterConfiguration._StandingWalkingSpeed;
	}

	_SpeedSpringDampingSystem.SetDesired(desired_speed);

	//Calculate the speed.
	const float32 speed{ _SpeedSpringDampingSystem.Update(delta_time) };

	//Determine the forward and right movement speeds.
	const float32 forward_movement_speed{ _CurrentInputState._Movement._X * speed };
	const float32 right_movement_speed{ _CurrentInputState._Movement._Y * speed };

	//Update the vertical velocity.
	if (_CharacterController->IsOnGround())
	{
		if (_CurrentInputState._JumpButtonPressed)
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
		_VerticalVelocity += -PhysicsConstants::GRAVITY * delta_time;
	}

	//Calculate the total displacement.
	Vector3<float32> total_displacement{ 0.0f, 0.0f, 0.0f };

	total_displacement += Vector3<float32>(0.0f, _VerticalVelocity, 0.0f) * delta_time;
	total_displacement += forward * forward_movement_speed * delta_time;
	total_displacement += right * right_movement_speed * delta_time;

	//Move the character controller.
	_CharacterController->Move(total_displacement);

	//Set the crouch desired position.
	_CrouchSpringDampingSystem.SetDesired(_CurrentInputState._IsCrouching ? _CharacterConfiguration._CrouchingHeight : _CharacterConfiguration._StandingHeight);

	//Calculate the current height.
	const float32 current_height{ _CrouchSpringDampingSystem.Update(delta_time) };

	//Resize the capsule height of the character controller.
	_CharacterController->ResizeCapsuleHeight(current_height);

	//Set the perceiver position/rotation.
	const WorldTransform world_transform{ _CharacterController->GetWorldPosition().GetAbsolutePosition() + Vector3<float32>(0.0f, current_height, 0.0f), _Rotation, 1.0f };

	Perceiver::Instance->SetWorldTransform(world_transform);
}

/*
*	Updates the current input state.
*/
void Player::UpdateCurrentInputState(const float32 delta_time) NOEXCEPT
{
	//Define constants.
	constexpr float32 GAMEPAD_ROTATION_SPEED{ 2.5f };
	constexpr float32 MOUSE_ROTATION_SPEED{ 100.0f };

	//Get the input states.
	const GamepadState *const RESTRICT gamepad_state{ InputSystem::Instance->GetGamepadState() };
	const KeyboardState *const RESTRICT keyboard_state{ InputSystem::Instance->GetKeyboardState() };
	const MouseState *const RESTRICT mouse_state{ InputSystem::Instance->GetMouseState() };

	//Hide the cursor.
	InputSystem::Instance->HideCursor();

	//Update the rotation.
	_CurrentInputState._Rotation._X = 0.0f;
	_CurrentInputState._Rotation._Y = 0.0f;

	_CurrentInputState._Rotation._X += -gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED;

	_CurrentInputState._Rotation._X += -mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;

	//Update the movement.
	Vector2<float32> desired_movement;

	desired_movement._X = 0.0f;
	desired_movement._Y = 0.0f;

	desired_movement._X += gamepad_state->_LeftThumbstickY;
	desired_movement._Y += gamepad_state->_LeftThumbstickX;

	desired_movement._X += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD);
	desired_movement._X -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD);

	desired_movement._Y += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD);
	desired_movement._Y -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD);

	for (uint8 i{ 0 }; i < 2; ++i)
	{
		_CurrentInputState._MovementSpringDampingSystems[i].SetDesired(desired_movement[i]);
		_CurrentInputState._Movement[i] = _CurrentInputState._MovementSpringDampingSystems[i].Update(delta_time);
	}

	//Update if the player is crouching.
	switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
	{
		case InputDeviceType::GAMEPAD:
		{
			if (gamepad_state->_B == ButtonState::PRESSED)
			{
				_CurrentInputState._GamepadCrouchToggle = !_CurrentInputState._GamepadCrouchToggle;
				_CurrentInputState._IsCrouching = _CurrentInputState._GamepadCrouchToggle;
			}

			break;
		}

		case InputDeviceType::KEYBOARD:
		case InputDeviceType::MOUSE:
		{
			_CurrentInputState._IsCrouching = keyboard_state->GetButtonState(KeyboardButton::C) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::C) == ButtonState::PRESSED_HELD;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Update if the player is running.
	switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
	{
		case InputDeviceType::GAMEPAD:
		{
			if (gamepad_state->_LeftThumb == ButtonState::PRESSED)
			{
				_CurrentInputState._GamepadRunToggle = !_CurrentInputState._GamepadRunToggle;
				_CurrentInputState._IsRunning = _CurrentInputState._GamepadRunToggle;
			}

			break;
		}

		case InputDeviceType::KEYBOARD:
		case InputDeviceType::MOUSE:
		{
			_CurrentInputState._IsRunning = keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED_HELD;

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}

	//Update if the jump button was pressed.
	_CurrentInputState._JumpButtonPressed = gamepad_state->_A == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Spacebar) == ButtonState::PRESSED;
}