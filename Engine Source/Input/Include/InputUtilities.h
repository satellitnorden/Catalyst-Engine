#pragma once

//Engine core.
#include <EngineCore.h>

//Input.
#include <GamepadState.h>

//Math.
#include <GameMath.h>

namespace InputUtilities
{

#if defined(CATALYST_WINDOWS)
	/*
	*	Updates a single gamepad button.
	*/
	void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, GamepadButtonState &gamepadButtonState) CATALYST_NOEXCEPT
	{
		//The gamepad button is not pressed.
		if (!(wButtons & xInputButton))
		{
			if (gamepadButtonState == GamepadButtonState::ReleasedHold)
			{
				//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
			}

			else if (gamepadButtonState == GamepadButtonState::PressedHold || gamepadButtonState == GamepadButtonState::Pressed)
				gamepadButtonState = GamepadButtonState::Released;

			else if (gamepadButtonState == GamepadButtonState::Released)
				gamepadButtonState = GamepadButtonState::ReleasedHold;
		}

		//The gamepad button is pressed.
		else
		{
			if (gamepadButtonState == GamepadButtonState::PressedHold)
			{
				//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
			}

			else if (gamepadButtonState == GamepadButtonState::ReleasedHold || gamepadButtonState == GamepadButtonState::Released)
				gamepadButtonState = GamepadButtonState::Pressed;

			else if (gamepadButtonState == GamepadButtonState::Pressed)
				gamepadButtonState = GamepadButtonState::PressedHold;
		}
	}

	/*
	*	Returns the current gamepad state.
	*/
	bool GetCurrentGamepadState(const uint8 index, const bool updateGamepadButtons, GamepadState &currentGamepadState) CATALYST_NOEXCEPT
	{
		XINPUT_STATE xInputState;

		if (XInputGetState(index, &xInputState) != ERROR_SUCCESS)
		{
			currentGamepadState.isConnected = false;

			return false;
		}
			
		else
		{
			currentGamepadState.isConnected = true;
		}

		if (updateGamepadButtons)
		{
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, currentGamepadState.dpadUpButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, currentGamepadState.dpadDownButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, currentGamepadState.dpadLeftButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, currentGamepadState.dpadRightButtonState);

			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, currentGamepadState.startButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, currentGamepadState.backButtonState);

			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, currentGamepadState.leftThumbButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, currentGamepadState.rightThumbButtonState);

			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, currentGamepadState.leftShoulderButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, currentGamepadState.rightShoulderButtonState);

			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, currentGamepadState.aButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, currentGamepadState.bButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, currentGamepadState.xButtonState);
			UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, currentGamepadState.yButtonState);
		}

		currentGamepadState.leftTriggerValue = static_cast<float>(xInputState.Gamepad.bLeftTrigger) / 255;
		currentGamepadState.rightTriggerValue = static_cast<float>(xInputState.Gamepad.bRightTrigger) / 255;

		currentGamepadState.leftThumbstickXValue = static_cast<float>(xInputState.Gamepad.sThumbLX) / 32'768;
		currentGamepadState.leftThumbstickYValue = static_cast<float>(xInputState.Gamepad.sThumbLY) / 32'768;

		currentGamepadState.rightThumbstickXValue = static_cast<float>(xInputState.Gamepad.sThumbRX) / 32'768;
		currentGamepadState.rightThumbstickYValue = static_cast<float>(xInputState.Gamepad.sThumbRY) / 32'768;

		//Calculate if the thumbstick values are in the deadzone.If so, reset them.
		if (GameMath::Absolute(currentGamepadState.leftThumbstickXValue) <= GAMEPAD_DEADZONE)
			currentGamepadState.leftThumbstickXValue = 0.0f;

		if (GameMath::Absolute(currentGamepadState.leftThumbstickYValue) <= GAMEPAD_DEADZONE)
			currentGamepadState.leftThumbstickYValue = 0.0f;

		if (GameMath::Absolute(currentGamepadState.rightThumbstickXValue) <= GAMEPAD_DEADZONE)
			currentGamepadState.rightThumbstickXValue = 0.0f;

		if (GameMath::Absolute(currentGamepadState.rightThumbstickYValue) <= GAMEPAD_DEADZONE)
			currentGamepadState.rightThumbstickYValue = 0.0f;

		//Return that the query succeeded.
		return true;
	}
#endif

}