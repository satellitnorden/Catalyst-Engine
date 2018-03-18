#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>

//Math.
#include <Math/CatalystMath.h>

namespace InputUtilities
{

#if defined(CATALYST_WINDOWS)
	/*
	*	Updates a single gamepad button.
	*/
	void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, GamepadButtonState &gamepadButtonState) NOEXCEPT
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
	*	Updates a single keyboard button.
	*/
	void UpdateKeyboardButton(const uint16 button, KeyboardButtonState &keyboardButtonState) NOEXCEPT
	{
		//The keyboard button is not pressed.
		if (!StaticCast<bool>(GetKeyState(button) & 0x8000))
		{
			if (keyboardButtonState == KeyboardButtonState::ReleasedHold)
			{
				//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
			}

			else if (keyboardButtonState == KeyboardButtonState::PressedHold || keyboardButtonState == KeyboardButtonState::Pressed)
				keyboardButtonState = KeyboardButtonState::Released;

			else if (keyboardButtonState == KeyboardButtonState::Released)
				keyboardButtonState = KeyboardButtonState::ReleasedHold;
		}

		//The keyboard button is pressed.
		else
		{
			if (keyboardButtonState == KeyboardButtonState::PressedHold)
			{
				//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
			}

			else if (keyboardButtonState == KeyboardButtonState::ReleasedHold || keyboardButtonState == KeyboardButtonState::Released)
				keyboardButtonState = KeyboardButtonState::Pressed;

			else if (keyboardButtonState == KeyboardButtonState::Pressed)
				keyboardButtonState = KeyboardButtonState::PressedHold;
		}
	}

	/*
	*	Returns the current gamepad state.
	*/
	bool GetCurrentGamepadState(const uint8 index, GamepadState &currentGamepadStates) NOEXCEPT
	{
		XINPUT_STATE xInputState;

		if (XInputGetState(index, &xInputState) != ERROR_SUCCESS)
		{
			currentGamepadStates.isConnected = false;

			return false;
		}
			
		else
		{
			currentGamepadStates.isConnected = true;
		}

		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, currentGamepadStates.dpadUpButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, currentGamepadStates.dpadDownButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, currentGamepadStates.dpadLeftButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, currentGamepadStates.dpadRightButtonState);

		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, currentGamepadStates.startButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, currentGamepadStates.backButtonState);

		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, currentGamepadStates.leftThumbButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, currentGamepadStates.rightThumbButtonState);

		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, currentGamepadStates.leftShoulderButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, currentGamepadStates.rightShoulderButtonState);

		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, currentGamepadStates.aButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, currentGamepadStates.bButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, currentGamepadStates.xButtonState);
		UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, currentGamepadStates.yButtonState);

		currentGamepadStates.leftTriggerValue = StaticCast<float>(xInputState.Gamepad.bLeftTrigger) / 255;
		currentGamepadStates.rightTriggerValue = StaticCast<float>(xInputState.Gamepad.bRightTrigger) / 255;

		currentGamepadStates.leftThumbstickXValue = StaticCast<float>(xInputState.Gamepad.sThumbLX) / 32'768;
		currentGamepadStates.leftThumbstickYValue = StaticCast<float>(xInputState.Gamepad.sThumbLY) / 32'768;

		currentGamepadStates.rightThumbstickXValue = StaticCast<float>(xInputState.Gamepad.sThumbRX) / 32'768;
		currentGamepadStates.rightThumbstickYValue = StaticCast<float>(xInputState.Gamepad.sThumbRY) / 32'768;

		//Calculate if the thumbstick values are in the deadzone.If so, reset them.
		if (CatalystMath::Absolute(currentGamepadStates.leftThumbstickXValue) <= GAMEPAD_DEADZONE)
			currentGamepadStates.leftThumbstickXValue = 0.0f;

		if (CatalystMath::Absolute(currentGamepadStates.leftThumbstickYValue) <= GAMEPAD_DEADZONE)
			currentGamepadStates.leftThumbstickYValue = 0.0f;

		if (CatalystMath::Absolute(currentGamepadStates.rightThumbstickXValue) <= GAMEPAD_DEADZONE)
			currentGamepadStates.rightThumbstickXValue = 0.0f;

		if (CatalystMath::Absolute(currentGamepadStates.rightThumbstickYValue) <= GAMEPAD_DEADZONE)
			currentGamepadStates.rightThumbstickYValue = 0.0f;

		//Return that the query succeeded.
		return true;
	}

	/*
	*	Returns the current keyboard state.
	*/
	bool GetCurrentKeyboardState(KeyboardState &currentKeyboardState) NOEXCEPT
	{
		UpdateKeyboardButton(VK_BACK, currentKeyboardState[KeyboardButton::Backspace]);
		UpdateKeyboardButton(VK_TAB, currentKeyboardState[KeyboardButton::Tab]);
		UpdateKeyboardButton(VK_CLEAR, currentKeyboardState[KeyboardButton::Clear]);
		UpdateKeyboardButton(VK_RETURN, currentKeyboardState[KeyboardButton::Enter]);
		UpdateKeyboardButton(VK_PAUSE, currentKeyboardState[KeyboardButton::Pause]);
		UpdateKeyboardButton(VK_CAPITAL, currentKeyboardState[KeyboardButton::CapsLock]);
		UpdateKeyboardButton(VK_ESCAPE, currentKeyboardState[KeyboardButton::Escape]);
		UpdateKeyboardButton(VK_SPACE, currentKeyboardState[KeyboardButton::Spacebar]);
		UpdateKeyboardButton(VK_PRIOR, currentKeyboardState[KeyboardButton::PageUp]);
		UpdateKeyboardButton(VK_NEXT, currentKeyboardState[KeyboardButton::PageDown]);
		UpdateKeyboardButton(VK_HOME, currentKeyboardState[KeyboardButton::Home]);
		UpdateKeyboardButton(VK_LEFT, currentKeyboardState[KeyboardButton::LeftArrow]);
		UpdateKeyboardButton(VK_UP, currentKeyboardState[KeyboardButton::UpArrow]);
		UpdateKeyboardButton(VK_RIGHT, currentKeyboardState[KeyboardButton::RightArrow]);
		UpdateKeyboardButton(VK_DOWN, currentKeyboardState[KeyboardButton::DownArrow]);
		UpdateKeyboardButton(VK_SELECT, currentKeyboardState[KeyboardButton::Select]);
		UpdateKeyboardButton(VK_PRINT, currentKeyboardState[KeyboardButton::Print]);
		UpdateKeyboardButton(VK_EXECUTE, currentKeyboardState[KeyboardButton::Execute]);
		UpdateKeyboardButton(VK_SNAPSHOT, currentKeyboardState[KeyboardButton::PrintScreen]);
		UpdateKeyboardButton(VK_INSERT, currentKeyboardState[KeyboardButton::Insert]);
		UpdateKeyboardButton(VK_DELETE, currentKeyboardState[KeyboardButton::Delete]);
		UpdateKeyboardButton(VK_HELP, currentKeyboardState[KeyboardButton::Help]);

		UpdateKeyboardButton(0x30, currentKeyboardState[KeyboardButton::Zero]);
		UpdateKeyboardButton(0x31, currentKeyboardState[KeyboardButton::One]);
		UpdateKeyboardButton(0x32, currentKeyboardState[KeyboardButton::Two]);
		UpdateKeyboardButton(0x33, currentKeyboardState[KeyboardButton::Three]);
		UpdateKeyboardButton(0x34, currentKeyboardState[KeyboardButton::Four]);
		UpdateKeyboardButton(0x35, currentKeyboardState[KeyboardButton::Five]);
		UpdateKeyboardButton(0x36, currentKeyboardState[KeyboardButton::Six]);
		UpdateKeyboardButton(0x37, currentKeyboardState[KeyboardButton::Seven]);
		UpdateKeyboardButton(0x38, currentKeyboardState[KeyboardButton::Eight]);
		UpdateKeyboardButton(0x39, currentKeyboardState[KeyboardButton::Nine]);

		UpdateKeyboardButton(0x41, currentKeyboardState[KeyboardButton::A]);
		UpdateKeyboardButton(0x42, currentKeyboardState[KeyboardButton::B]);
		UpdateKeyboardButton(0x43, currentKeyboardState[KeyboardButton::C]);
		UpdateKeyboardButton(0x44, currentKeyboardState[KeyboardButton::D]);
		UpdateKeyboardButton(0x45, currentKeyboardState[KeyboardButton::E]);
		UpdateKeyboardButton(0x46, currentKeyboardState[KeyboardButton::F]);
		UpdateKeyboardButton(0x47, currentKeyboardState[KeyboardButton::G]);
		UpdateKeyboardButton(0x48, currentKeyboardState[KeyboardButton::H]);
		UpdateKeyboardButton(0x49, currentKeyboardState[KeyboardButton::I]);
		UpdateKeyboardButton(0x4A, currentKeyboardState[KeyboardButton::J]);
		UpdateKeyboardButton(0x4B, currentKeyboardState[KeyboardButton::K]);
		UpdateKeyboardButton(0x4C, currentKeyboardState[KeyboardButton::L]);
		UpdateKeyboardButton(0x4D, currentKeyboardState[KeyboardButton::M]);
		UpdateKeyboardButton(0x4E, currentKeyboardState[KeyboardButton::N]);
		UpdateKeyboardButton(0x4F, currentKeyboardState[KeyboardButton::O]);
		UpdateKeyboardButton(0x50, currentKeyboardState[KeyboardButton::P]);
		UpdateKeyboardButton(0x51, currentKeyboardState[KeyboardButton::Q]);
		UpdateKeyboardButton(0x52, currentKeyboardState[KeyboardButton::R]);
		UpdateKeyboardButton(0x53, currentKeyboardState[KeyboardButton::S]);
		UpdateKeyboardButton(0x54, currentKeyboardState[KeyboardButton::T]);
		UpdateKeyboardButton(0x55, currentKeyboardState[KeyboardButton::U]);
		UpdateKeyboardButton(0x56, currentKeyboardState[KeyboardButton::V]);
		UpdateKeyboardButton(0x57, currentKeyboardState[KeyboardButton::W]);
		UpdateKeyboardButton(0x58, currentKeyboardState[KeyboardButton::X]);
		UpdateKeyboardButton(0x59, currentKeyboardState[KeyboardButton::Y]);
		UpdateKeyboardButton(0x5A, currentKeyboardState[KeyboardButton::Z]);

		UpdateKeyboardButton(VK_LWIN, currentKeyboardState[KeyboardButton::LeftWindows]);
		UpdateKeyboardButton(VK_RWIN, currentKeyboardState[KeyboardButton::RightWindows]);
		UpdateKeyboardButton(VK_APPS, currentKeyboardState[KeyboardButton::Applications]);
		UpdateKeyboardButton(VK_SLEEP, currentKeyboardState[KeyboardButton::Sleep]);

		UpdateKeyboardButton(VK_NUMPAD0, currentKeyboardState[KeyboardButton::NumpadZero]);
		UpdateKeyboardButton(VK_NUMPAD1, currentKeyboardState[KeyboardButton::NumpadOne]);
		UpdateKeyboardButton(VK_NUMPAD2, currentKeyboardState[KeyboardButton::NumpadTwo]);
		UpdateKeyboardButton(VK_NUMPAD3, currentKeyboardState[KeyboardButton::NumpadThree]);
		UpdateKeyboardButton(VK_NUMPAD4, currentKeyboardState[KeyboardButton::NumpadFour]);
		UpdateKeyboardButton(VK_NUMPAD5, currentKeyboardState[KeyboardButton::NumpadFive]);
		UpdateKeyboardButton(VK_NUMPAD6, currentKeyboardState[KeyboardButton::NumpadSix]);
		UpdateKeyboardButton(VK_NUMPAD7, currentKeyboardState[KeyboardButton::NumpadSeven]);
		UpdateKeyboardButton(VK_NUMPAD8, currentKeyboardState[KeyboardButton::NumpadEight]);
		UpdateKeyboardButton(VK_NUMPAD9, currentKeyboardState[KeyboardButton::NumpadNine]);

		UpdateKeyboardButton(VK_MULTIPLY, currentKeyboardState[KeyboardButton::NumpadMultiply]);
		UpdateKeyboardButton(VK_ADD, currentKeyboardState[KeyboardButton::NumpadAdd]);
		UpdateKeyboardButton(VK_SEPARATOR, currentKeyboardState[KeyboardButton::NumpadSeparator]);
		UpdateKeyboardButton(VK_SUBTRACT, currentKeyboardState[KeyboardButton::NumpadSubtract]);
		UpdateKeyboardButton(VK_DECIMAL, currentKeyboardState[KeyboardButton::NumpadDecimal]);
		UpdateKeyboardButton(VK_DIVIDE, currentKeyboardState[KeyboardButton::NumpadDivide]);

		UpdateKeyboardButton(VK_F1, currentKeyboardState[KeyboardButton::F1]);
		UpdateKeyboardButton(VK_F2, currentKeyboardState[KeyboardButton::F2]);
		UpdateKeyboardButton(VK_F3, currentKeyboardState[KeyboardButton::F3]);
		UpdateKeyboardButton(VK_F4, currentKeyboardState[KeyboardButton::F4]);
		UpdateKeyboardButton(VK_F5, currentKeyboardState[KeyboardButton::F5]);
		UpdateKeyboardButton(VK_F6, currentKeyboardState[KeyboardButton::F6]);
		UpdateKeyboardButton(VK_F7, currentKeyboardState[KeyboardButton::F7]);
		UpdateKeyboardButton(VK_F8, currentKeyboardState[KeyboardButton::F8]);
		UpdateKeyboardButton(VK_F9, currentKeyboardState[KeyboardButton::F9]);
		UpdateKeyboardButton(VK_F10, currentKeyboardState[KeyboardButton::F10]);
		UpdateKeyboardButton(VK_F11, currentKeyboardState[KeyboardButton::F11]);
		UpdateKeyboardButton(VK_F12, currentKeyboardState[KeyboardButton::F12]);
		UpdateKeyboardButton(VK_F13, currentKeyboardState[KeyboardButton::F13]);
		UpdateKeyboardButton(VK_F14, currentKeyboardState[KeyboardButton::F14]);
		UpdateKeyboardButton(VK_F15, currentKeyboardState[KeyboardButton::F15]);
		UpdateKeyboardButton(VK_F16, currentKeyboardState[KeyboardButton::F16]);
		UpdateKeyboardButton(VK_F17, currentKeyboardState[KeyboardButton::F17]);
		UpdateKeyboardButton(VK_F18, currentKeyboardState[KeyboardButton::F18]);
		UpdateKeyboardButton(VK_F19, currentKeyboardState[KeyboardButton::F19]);
		UpdateKeyboardButton(VK_F20, currentKeyboardState[KeyboardButton::F20]);
		UpdateKeyboardButton(VK_F21, currentKeyboardState[KeyboardButton::F21]);
		UpdateKeyboardButton(VK_F22, currentKeyboardState[KeyboardButton::F22]);
		UpdateKeyboardButton(VK_F23, currentKeyboardState[KeyboardButton::F23]);
		UpdateKeyboardButton(VK_F24, currentKeyboardState[KeyboardButton::F24]);

		UpdateKeyboardButton(VK_NUMLOCK, currentKeyboardState[KeyboardButton::NumLock]);
		UpdateKeyboardButton(VK_SCROLL, currentKeyboardState[KeyboardButton::ScrollLock]);
		UpdateKeyboardButton(VK_LSHIFT, currentKeyboardState[KeyboardButton::LeftShift]);
		UpdateKeyboardButton(VK_RSHIFT, currentKeyboardState[KeyboardButton::RightShift]);
		UpdateKeyboardButton(VK_LCONTROL, currentKeyboardState[KeyboardButton::LeftControl]);
		UpdateKeyboardButton(VK_RCONTROL, currentKeyboardState[KeyboardButton::RightControl]);
		UpdateKeyboardButton(VK_LMENU, currentKeyboardState[KeyboardButton::LeftAlt]);
		UpdateKeyboardButton(VK_RMENU, currentKeyboardState[KeyboardButton::RightAlt]);

		UpdateKeyboardButton(VK_BROWSER_BACK, currentKeyboardState[KeyboardButton::BrowserBack]);
		UpdateKeyboardButton(VK_BROWSER_FORWARD, currentKeyboardState[KeyboardButton::BrowserForward]);
		UpdateKeyboardButton(VK_BROWSER_REFRESH, currentKeyboardState[KeyboardButton::BrowserRefresh]);
		UpdateKeyboardButton(VK_BROWSER_STOP, currentKeyboardState[KeyboardButton::BrowserStop]);
		UpdateKeyboardButton(VK_BROWSER_SEARCH, currentKeyboardState[KeyboardButton::BrowserSearch]);
		UpdateKeyboardButton(VK_BROWSER_FAVORITES, currentKeyboardState[KeyboardButton::BrowserFavorites]);
		UpdateKeyboardButton(VK_BROWSER_HOME, currentKeyboardState[KeyboardButton::BrowserStartAndHome]);

		UpdateKeyboardButton(VK_VOLUME_MUTE, currentKeyboardState[KeyboardButton::VolumeMute]);
		UpdateKeyboardButton(VK_VOLUME_DOWN, currentKeyboardState[KeyboardButton::VolumeDown]);
		UpdateKeyboardButton(VK_VOLUME_UP, currentKeyboardState[KeyboardButton::VolumeUp]);

		UpdateKeyboardButton(VK_MEDIA_NEXT_TRACK, currentKeyboardState[KeyboardButton::NextTrack]);
		UpdateKeyboardButton(VK_MEDIA_PREV_TRACK, currentKeyboardState[KeyboardButton::PreviousTrack]);
		UpdateKeyboardButton(VK_MEDIA_STOP, currentKeyboardState[KeyboardButton::StopMedia]);
		UpdateKeyboardButton(VK_MEDIA_PLAY_PAUSE, currentKeyboardState[KeyboardButton::PlayPause]);

		return true;
	}
#endif

}