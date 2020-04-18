#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/InputSystem.h>

//Windows.
#include <Xinput.h>

/*
*	Updates a single gamepad button.
*/
FORCE_INLINE void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, ButtonState &ButtonState) NOEXCEPT
{
	//The gamepad button is not pressed.
	if (!(wButtons & xInputButton))
	{
		if (ButtonState == ButtonState::RELEASED_HELD)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::PRESSED_HELD || ButtonState == ButtonState::PRESSED)
		{
			ButtonState = ButtonState::RELEASED;
		}

		else if (ButtonState == ButtonState::RELEASED)
		{
			ButtonState = ButtonState::RELEASED_HELD;
		}	
	}

	//The gamepad button is pressed.
	else
	{
		if (ButtonState == ButtonState::PRESSED_HELD)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::RELEASED_HELD || ButtonState == ButtonState::RELEASED)
		{
			ButtonState = ButtonState::PRESSED;
		}

		else if (ButtonState == ButtonState::PRESSED)
		{
			ButtonState = ButtonState::PRESSED_HELD;
		}
	}
}

/*
*	Updates the gamepad state at the given index.
*/
void InputSystem::UpdateGamepadState(const uint8 index) NOEXCEPT
{
	//The COM library needs to be initialized for XInput, so initialize that if not already done.
	static thread_local bool com_initialized{ false };

	if (!com_initialized)
	{
		CoInitialize(nullptr);

		com_initialized = true;
	}

	//No need to update if the window isn't in focus.
	if (!CatalystPlatform::IsWindowInFocus())
	{
		return;
	}

	//Cache the state.
	GamepadState *const RESTRICT state{ &_InputState._GamepadStates[index] };

	XINPUT_STATE xInputState;

	if (XInputGetState(index, &xInputState) != ERROR_SUCCESS)
	{
		state->_IsConnected = false;

		return;
	}

	else
	{
		state->_IsConnected = true;
	}

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, state->_DpadUp);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, state->_DpadDown);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, state->_DpadLeft);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, state->_DpadRight);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, state->_Start);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, state->_Back);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, state->_LeftThumb);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, state->_RightThumb);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, state->_LeftShoulder);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, state->_RightShoulder);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, state->_A);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, state->_B);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, state->_X);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, state->_Y);

	state->_LeftTrigger = static_cast<float>(xInputState.Gamepad.bLeftTrigger) / 255;
	state->_RightTrigger = static_cast<float>(xInputState.Gamepad.bRightTrigger) / 255;

	state->_LeftThumbstickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / 32'768;
	state->_LeftThumbstickY = static_cast<float>(xInputState.Gamepad.sThumbLY) / 32'768;

	state->_RightThumbstickX = static_cast<float>(xInputState.Gamepad.sThumbRX) / 32'768;
	state->_RightThumbstickY = static_cast<float>(xInputState.Gamepad.sThumbRY) / 32'768;

	//Calculate if the thumbstick values are in the deadzone.If so, reset them.
	if (CatalystBaseMath::Absolute(state->_LeftThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickX = 0.0f;
	}

	else
	{
		state->_LeftThumbstickX = CatalystBaseMath::Scale(state->_LeftThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (CatalystBaseMath::Absolute(state->_LeftThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickY = 0.0f;
	}

	else
	{
		state->_LeftThumbstickY = CatalystBaseMath::Scale(state->_LeftThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (CatalystBaseMath::Absolute(state->_RightThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickX = 0.0f;
	}

	else
	{
		state->_RightThumbstickX = CatalystBaseMath::Scale(state->_RightThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (CatalystBaseMath::Absolute(state->_RightThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickY = 0.0f;
	}

	else
	{
		state->_RightThumbstickY = CatalystBaseMath::Scale(state->_RightThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}
}

/*
*	Updates a single Windows button.
*/
FORCE_INLINE void UpdateWindowsButton(const uint16 button, ButtonState &ButtonState) NOEXCEPT
{
	//The Windows button is not pressed.
	if (!static_cast<bool>(GetKeyState(button) & 0x8000))
	{
		if (ButtonState == ButtonState::RELEASED_HELD)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::PRESSED_HELD || ButtonState == ButtonState::PRESSED)
		{
			ButtonState = ButtonState::RELEASED;
		}

		else if (ButtonState == ButtonState::RELEASED)
		{
			ButtonState = ButtonState::RELEASED_HELD;
		}
	}

	//The Windows button is pressed.
	else
	{
		if (ButtonState == ButtonState::PRESSED_HELD)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::RELEASED_HELD || ButtonState == ButtonState::RELEASED)
		{
			ButtonState = ButtonState::PRESSED;
		}

		else if (ButtonState == ButtonState::PRESSED)
		{
			ButtonState = ButtonState::PRESSED_HELD;
		}
	}
}

/*
*	Updates the keyboard state.
*/
void InputSystem::UpdateKeyboardState() NOEXCEPT
{
	//No need to update if the window isn't in focus.
	if (!CatalystPlatform::IsWindowInFocus())
	{
		return;
	}

	//Cache the state.
	KeyboardState* const RESTRICT state{ &_InputState._KeyboardState };

	UpdateWindowsButton(VK_BACK, (*state)[KeyboardButton::Backspace]);
	UpdateWindowsButton(VK_TAB, (*state)[KeyboardButton::Tab]);
	UpdateWindowsButton(VK_CLEAR, (*state)[KeyboardButton::Clear]);
	UpdateWindowsButton(VK_RETURN, (*state)[KeyboardButton::Enter]);
	UpdateWindowsButton(VK_PAUSE, (*state)[KeyboardButton::Pause]);
	UpdateWindowsButton(VK_CAPITAL, (*state)[KeyboardButton::CapsLock]);
	UpdateWindowsButton(VK_ESCAPE, (*state)[KeyboardButton::Escape]);
	UpdateWindowsButton(VK_SPACE, (*state)[KeyboardButton::Spacebar]);
	UpdateWindowsButton(VK_PRIOR, (*state)[KeyboardButton::PageUp]);
	UpdateWindowsButton(VK_NEXT, (*state)[KeyboardButton::PageDown]);
	UpdateWindowsButton(VK_HOME, (*state)[KeyboardButton::Home]);
	UpdateWindowsButton(VK_LEFT, (*state)[KeyboardButton::LeftArrow]);
	UpdateWindowsButton(VK_UP, (*state)[KeyboardButton::UpArrow]);
	UpdateWindowsButton(VK_RIGHT, (*state)[KeyboardButton::RightArrow]);
	UpdateWindowsButton(VK_DOWN, (*state)[KeyboardButton::DownArrow]);
	UpdateWindowsButton(VK_SELECT, (*state)[KeyboardButton::Select]);
	UpdateWindowsButton(VK_PRINT, (*state)[KeyboardButton::Print]);
	UpdateWindowsButton(VK_EXECUTE, (*state)[KeyboardButton::Execute]);
	UpdateWindowsButton(VK_SNAPSHOT, (*state)[KeyboardButton::PrintScreen]);
	UpdateWindowsButton(VK_INSERT, (*state)[KeyboardButton::Insert]);
	UpdateWindowsButton(VK_DELETE, (*state)[KeyboardButton::Delete]);
	UpdateWindowsButton(VK_HELP, (*state)[KeyboardButton::Help]);

	UpdateWindowsButton(0x30, (*state)[KeyboardButton::Zero]);
	UpdateWindowsButton(0x31, (*state)[KeyboardButton::One]);
	UpdateWindowsButton(0x32, (*state)[KeyboardButton::Two]);
	UpdateWindowsButton(0x33, (*state)[KeyboardButton::Three]);
	UpdateWindowsButton(0x34, (*state)[KeyboardButton::Four]);
	UpdateWindowsButton(0x35, (*state)[KeyboardButton::Five]);
	UpdateWindowsButton(0x36, (*state)[KeyboardButton::Six]);
	UpdateWindowsButton(0x37, (*state)[KeyboardButton::Seven]);
	UpdateWindowsButton(0x38, (*state)[KeyboardButton::Eight]);
	UpdateWindowsButton(0x39, (*state)[KeyboardButton::Nine]);

	UpdateWindowsButton(0x41, (*state)[KeyboardButton::A]);
	UpdateWindowsButton(0x42, (*state)[KeyboardButton::B]);
	UpdateWindowsButton(0x43, (*state)[KeyboardButton::C]);
	UpdateWindowsButton(0x44, (*state)[KeyboardButton::D]);
	UpdateWindowsButton(0x45, (*state)[KeyboardButton::E]);
	UpdateWindowsButton(0x46, (*state)[KeyboardButton::F]);
	UpdateWindowsButton(0x47, (*state)[KeyboardButton::G]);
	UpdateWindowsButton(0x48, (*state)[KeyboardButton::H]);
	UpdateWindowsButton(0x49, (*state)[KeyboardButton::I]);
	UpdateWindowsButton(0x4A, (*state)[KeyboardButton::J]);
	UpdateWindowsButton(0x4B, (*state)[KeyboardButton::K]);
	UpdateWindowsButton(0x4C, (*state)[KeyboardButton::L]);
	UpdateWindowsButton(0x4D, (*state)[KeyboardButton::M]);
	UpdateWindowsButton(0x4E, (*state)[KeyboardButton::N]);
	UpdateWindowsButton(0x4F, (*state)[KeyboardButton::O]);
	UpdateWindowsButton(0x50, (*state)[KeyboardButton::P]);
	UpdateWindowsButton(0x51, (*state)[KeyboardButton::Q]);
	UpdateWindowsButton(0x52, (*state)[KeyboardButton::R]);
	UpdateWindowsButton(0x53, (*state)[KeyboardButton::S]);
	UpdateWindowsButton(0x54, (*state)[KeyboardButton::T]);
	UpdateWindowsButton(0x55, (*state)[KeyboardButton::U]);
	UpdateWindowsButton(0x56, (*state)[KeyboardButton::V]);
	UpdateWindowsButton(0x57, (*state)[KeyboardButton::W]);
	UpdateWindowsButton(0x58, (*state)[KeyboardButton::X]);
	UpdateWindowsButton(0x59, (*state)[KeyboardButton::Y]);
	UpdateWindowsButton(0x5A, (*state)[KeyboardButton::Z]);

	UpdateWindowsButton(VK_LWIN, (*state)[KeyboardButton::LeftWindows]);
	UpdateWindowsButton(VK_RWIN, (*state)[KeyboardButton::RightWindows]);
	UpdateWindowsButton(VK_APPS, (*state)[KeyboardButton::Applications]);
	UpdateWindowsButton(VK_SLEEP, (*state)[KeyboardButton::Sleep]);

	UpdateWindowsButton(VK_NUMPAD0, (*state)[KeyboardButton::NumpadZero]);
	UpdateWindowsButton(VK_NUMPAD1, (*state)[KeyboardButton::NumpadOne]);
	UpdateWindowsButton(VK_NUMPAD2, (*state)[KeyboardButton::NumpadTwo]);
	UpdateWindowsButton(VK_NUMPAD3, (*state)[KeyboardButton::NumpadThree]);
	UpdateWindowsButton(VK_NUMPAD4, (*state)[KeyboardButton::NumpadFour]);
	UpdateWindowsButton(VK_NUMPAD5, (*state)[KeyboardButton::NumpadFive]);
	UpdateWindowsButton(VK_NUMPAD6, (*state)[KeyboardButton::NumpadSix]);
	UpdateWindowsButton(VK_NUMPAD7, (*state)[KeyboardButton::NumpadSeven]);
	UpdateWindowsButton(VK_NUMPAD8, (*state)[KeyboardButton::NumpadEight]);
	UpdateWindowsButton(VK_NUMPAD9, (*state)[KeyboardButton::NumpadNine]);

	UpdateWindowsButton(VK_MULTIPLY, (*state)[KeyboardButton::NumpadMultiply]);
	UpdateWindowsButton(VK_ADD, (*state)[KeyboardButton::NumpadAdd]);
	UpdateWindowsButton(VK_SEPARATOR, (*state)[KeyboardButton::NumpadSeparator]);
	UpdateWindowsButton(VK_SUBTRACT, (*state)[KeyboardButton::NumpadSubtract]);
	UpdateWindowsButton(VK_DECIMAL, (*state)[KeyboardButton::NumpadDecimal]);
	UpdateWindowsButton(VK_DIVIDE, (*state)[KeyboardButton::NumpadDivide]);

	UpdateWindowsButton(VK_F1, (*state)[KeyboardButton::F1]);
	UpdateWindowsButton(VK_F2, (*state)[KeyboardButton::F2]);
	UpdateWindowsButton(VK_F3, (*state)[KeyboardButton::F3]);
	UpdateWindowsButton(VK_F4, (*state)[KeyboardButton::F4]);
	UpdateWindowsButton(VK_F5, (*state)[KeyboardButton::F5]);
	UpdateWindowsButton(VK_F6, (*state)[KeyboardButton::F6]);
	UpdateWindowsButton(VK_F7, (*state)[KeyboardButton::F7]);
	UpdateWindowsButton(VK_F8, (*state)[KeyboardButton::F8]);
	UpdateWindowsButton(VK_F9, (*state)[KeyboardButton::F9]);
	UpdateWindowsButton(VK_F10, (*state)[KeyboardButton::F10]);
	UpdateWindowsButton(VK_F11, (*state)[KeyboardButton::F11]);
	UpdateWindowsButton(VK_F12, (*state)[KeyboardButton::F12]);
	UpdateWindowsButton(VK_F13, (*state)[KeyboardButton::F13]);
	UpdateWindowsButton(VK_F14, (*state)[KeyboardButton::F14]);
	UpdateWindowsButton(VK_F15, (*state)[KeyboardButton::F15]);
	UpdateWindowsButton(VK_F16, (*state)[KeyboardButton::F16]);
	UpdateWindowsButton(VK_F17, (*state)[KeyboardButton::F17]);
	UpdateWindowsButton(VK_F18, (*state)[KeyboardButton::F18]);
	UpdateWindowsButton(VK_F19, (*state)[KeyboardButton::F19]);
	UpdateWindowsButton(VK_F20, (*state)[KeyboardButton::F20]);
	UpdateWindowsButton(VK_F21, (*state)[KeyboardButton::F21]);
	UpdateWindowsButton(VK_F22, (*state)[KeyboardButton::F22]);
	UpdateWindowsButton(VK_F23, (*state)[KeyboardButton::F23]);
	UpdateWindowsButton(VK_F24, (*state)[KeyboardButton::F24]);

	UpdateWindowsButton(VK_NUMLOCK, (*state)[KeyboardButton::NumLock]);
	UpdateWindowsButton(VK_SCROLL, (*state)[KeyboardButton::ScrollLock]);
	UpdateWindowsButton(VK_LSHIFT, (*state)[KeyboardButton::LeftShift]);
	UpdateWindowsButton(VK_RSHIFT, (*state)[KeyboardButton::RightShift]);
	UpdateWindowsButton(VK_LCONTROL, (*state)[KeyboardButton::LeftControl]);
	UpdateWindowsButton(VK_RCONTROL, (*state)[KeyboardButton::RightControl]);
	UpdateWindowsButton(VK_LMENU, (*state)[KeyboardButton::LeftAlt]);
	UpdateWindowsButton(VK_RMENU, (*state)[KeyboardButton::RightAlt]);

	UpdateWindowsButton(VK_BROWSER_BACK, (*state)[KeyboardButton::BrowserBack]);
	UpdateWindowsButton(VK_BROWSER_FORWARD, (*state)[KeyboardButton::BrowserForward]);
	UpdateWindowsButton(VK_BROWSER_REFRESH, (*state)[KeyboardButton::BrowserRefresh]);
	UpdateWindowsButton(VK_BROWSER_STOP, (*state)[KeyboardButton::BrowserStop]);
	UpdateWindowsButton(VK_BROWSER_SEARCH, (*state)[KeyboardButton::BrowserSearch]);
	UpdateWindowsButton(VK_BROWSER_FAVORITES, (*state)[KeyboardButton::BrowserFavorites]);
	UpdateWindowsButton(VK_BROWSER_HOME, (*state)[KeyboardButton::BrowserStartAndHome]);

	UpdateWindowsButton(VK_VOLUME_MUTE, (*state)[KeyboardButton::VolumeMute]);
	UpdateWindowsButton(VK_VOLUME_DOWN, (*state)[KeyboardButton::VolumeDown]);
	UpdateWindowsButton(VK_VOLUME_UP, (*state)[KeyboardButton::VolumeUp]);

	UpdateWindowsButton(VK_MEDIA_NEXT_TRACK, (*state)[KeyboardButton::NextTrack]);
	UpdateWindowsButton(VK_MEDIA_PREV_TRACK, (*state)[KeyboardButton::PreviousTrack]);
	UpdateWindowsButton(VK_MEDIA_STOP, (*state)[KeyboardButton::StopMedia]);
	UpdateWindowsButton(VK_MEDIA_PLAY_PAUSE, (*state)[KeyboardButton::PlayPause]);
}

/*
*	Updates the mouse state.
*/
void InputSystem::UpdateMouseState() NOEXCEPT
{

}
#endif