//Header file.
#include <Gameplay/Player.h>

//Math.
#include <Math/Core/CatalystCoordinateSpaces.h>

//Physics.
#include <Physics/Native/PhysicsCore.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/RenderingSystem.h>
#if USE_TEMPORARY_TERRAIN_SOLUTION
#include <Systems/TerrainSystem.h>
#endif

/*
*	Initialize this player.
*/
void Player::InitializePlayer() NOEXCEPT
{
	//Initialize this character.
	InitializeCharacter();
}

/*
*	Updates this player.
*/
void Player::UpdatePlayer(const float32 delta_time) NOEXCEPT
{
	//Update this character.
	UpdateCharacter(delta_time);

	if (_Enabled)
	{
		//Update the current input state.
		UpdateCurrentInputState(delta_time);

		//Apply rotation.
		_Rotation._Roll += _CurrentInputState._Rotation._X * delta_time;
		_Rotation._Yaw += _CurrentInputState._Rotation._Y * delta_time;
		_Rotation._Roll = CatalystBaseMath::Clamp<float32>(_Rotation._Roll, CatalystBaseMath::DegreesToRadians(-89.0f), CatalystBaseMath::DegreesToRadians(89.0f));

		//Calculate the forward and right vector.
		Vector3<float32> forward{ CatalystCoordinateSpacesUtilities::RotatedWorldForwardVector(_Rotation) };
		Vector3<float32> right{ CatalystCoordinateSpacesUtilities::RotatedWorldRightVector(_Rotation) };

		//Ignore the Y axis on the forward and right vectors.
		forward.NormalizeXZ();
		right.NormalizeXZ();

		//Add the movement.
		AddMovement(forward * _CurrentInputState._Movement._X);
		AddMovement(right * _CurrentInputState._Movement._Y);

		//Add jump.
		if (_CurrentInputState._JumpButtonPressed)
		{
			SetWantsToJump(true);
		}

		//Set whether the character wants to crouch/sprint.
		SetWantsToCrouch(_CurrentInputState._IsCrouching);
		SetWantsToSprint(_CurrentInputState._IsSprinting);

		//Hide the cursor.
		InputSystem::Instance->HideCursor();
	}

	else
	{
		//Show the cursor.
		InputSystem::Instance->ShowCursor();
	}

	//Post update the character.
	PostUpdateCharacter(delta_time);

	if (_Enabled)
	{
		//Set the camera position/rotation.
		const Vector3<float32> character_controller_position{ _CharacterController->GetWorldPosition().GetAbsolutePosition() };

		const WorldTransform world_transform{ character_controller_position + Vector3<float32>(0.0f, _CurrentHeight, 0.0f), _Rotation, 1.0f };

		RenderingSystem::Instance->GetCurrentCamera()->SetWorldTransform(world_transform);
	}
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

	//Update the rotation.
	_CurrentInputState._Rotation._X = 0.0f;
	_CurrentInputState._Rotation._Y = 0.0f;

	_CurrentInputState._Rotation._X += -gamepad_state->_RightThumbstickY * GAMEPAD_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += gamepad_state->_RightThumbstickX * GAMEPAD_ROTATION_SPEED;

	_CurrentInputState._Rotation._X += -mouse_state->_DeltaY * MOUSE_ROTATION_SPEED;
	_CurrentInputState._Rotation._Y += mouse_state->_DeltaX * MOUSE_ROTATION_SPEED;

	//Update the movement.
	_CurrentInputState._Movement._X = 0.0f;
	_CurrentInputState._Movement._Y = 0.0f;

	_CurrentInputState._Movement._X += gamepad_state->_LeftThumbstickY;
	_CurrentInputState._Movement._Y += gamepad_state->_LeftThumbstickX;

	_CurrentInputState._Movement._X += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::W) == ButtonState::PRESSED_HELD);
	_CurrentInputState._Movement._X -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::S) == ButtonState::PRESSED_HELD);

	_CurrentInputState._Movement._Y += static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::D) == ButtonState::PRESSED_HELD);
	_CurrentInputState._Movement._Y -= static_cast<float>(keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::A) == ButtonState::PRESSED_HELD);

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
	}

	//Update if the player is sprinting.
	switch (InputSystem::Instance->GetLastUpdatedInputDeviceType())
	{
		case InputDeviceType::GAMEPAD:
		{
			if (gamepad_state->_LeftThumb == ButtonState::PRESSED)
			{
				_CurrentInputState._GamepadSprintToggle = !_CurrentInputState._GamepadSprintToggle;
				_CurrentInputState._IsSprinting = _CurrentInputState._GamepadSprintToggle;
			}

			break;
		}

		case InputDeviceType::KEYBOARD:
		case InputDeviceType::MOUSE:
		{
			_CurrentInputState._IsSprinting = keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::LeftShift) == ButtonState::PRESSED_HELD;

			break;
		}
	}

	//Update if the jump button was pressed.
	_CurrentInputState._JumpButtonPressed = gamepad_state->_A == ButtonState::PRESSED || keyboard_state->GetButtonState(KeyboardButton::Spacebar) == ButtonState::PRESSED;
}