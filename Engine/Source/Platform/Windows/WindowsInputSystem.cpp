#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/InputSystem.h>

//Windows.
#include <Xinput.h>

//Windows input system data.
namespace WindowsInputSystemData
{
	//The last known cursor position.
	Vector2<float> _LastKnownCursorPosition{ 0.5f, 0.5f };
}

//Windows input system logic.
namespace WindowsInputSystemLogic
{
	/*
	*	Updates a single gamepad button.
	*/
	FORCE_INLINE void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, ButtonState *const RESTRICT button_state) NOEXCEPT
	{
		//The gamepad button is not pressed.
		if (!(wButtons & xInputButton))
		{
			if (*button_state == ButtonState::PRESSED_HELD || *button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::RELEASED;
			}

			else if (*button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::RELEASED_HELD;
			}	
		}

		//The gamepad button is pressed.
		else
		{
			if (*button_state == ButtonState::RELEASED_HELD || *button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::PRESSED;
			}

			else if (*button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::PRESSED_HELD;
			}
		}
	}

	/*
	*	Updates a single Windows button.
	*/
	FORCE_INLINE void UpdateWindowsButton(const uint16 button, ButtonState *const RESTRICT button_state) NOEXCEPT
	{
		//The Windows button is not pressed.
		if (!static_cast<bool>(GetKeyState(button) & 0x8000))
		{
			if (*button_state == ButtonState::PRESSED_HELD || *button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::RELEASED;
			}

			else if (*button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::RELEASED_HELD;
			}
		}

		//The Windows button is pressed.
		else
		{
			if (*button_state == ButtonState::RELEASED_HELD || *button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::PRESSED;
			}

			else if (*button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::PRESSED_HELD;
			}
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

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, &state->_DpadUp);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, &state->_DpadDown);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, &state->_DpadLeft);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, &state->_DpadRight);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, &state->_Start);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, &state->_Back);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, &state->_LeftThumb);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, &state->_RightThumb);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, &state->_LeftShoulder);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, &state->_RightShoulder);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, &state->_A);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, &state->_B);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, &state->_X);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, &state->_Y);

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

	WindowsInputSystemLogic::UpdateWindowsButton(VK_BACK, &(*state)[KeyboardButton::Backspace]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_TAB, &(*state)[KeyboardButton::Tab]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_CLEAR, &(*state)[KeyboardButton::Clear]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RETURN, &(*state)[KeyboardButton::Enter]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_PAUSE, &(*state)[KeyboardButton::Pause]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_CAPITAL, &(*state)[KeyboardButton::CapsLock]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_ESCAPE, &(*state)[KeyboardButton::Escape]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SPACE, &(*state)[KeyboardButton::Spacebar]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_PRIOR, &(*state)[KeyboardButton::PageUp]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NEXT, &(*state)[KeyboardButton::PageDown]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_HOME, &(*state)[KeyboardButton::Home]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_LEFT, &(*state)[KeyboardButton::LeftArrow]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_UP, &(*state)[KeyboardButton::UpArrow]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RIGHT, &(*state)[KeyboardButton::RightArrow]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_DOWN, &(*state)[KeyboardButton::DownArrow]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SELECT, &(*state)[KeyboardButton::Select]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_PRINT, &(*state)[KeyboardButton::Print]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_EXECUTE, &(*state)[KeyboardButton::Execute]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SNAPSHOT, &(*state)[KeyboardButton::PrintScreen]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_INSERT, &(*state)[KeyboardButton::Insert]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_DELETE, &(*state)[KeyboardButton::Delete]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_HELP, &(*state)[KeyboardButton::Help]);

	WindowsInputSystemLogic::UpdateWindowsButton(0x30, &(*state)[KeyboardButton::Zero]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x31, &(*state)[KeyboardButton::One]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x32, &(*state)[KeyboardButton::Two]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x33, &(*state)[KeyboardButton::Three]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x34, &(*state)[KeyboardButton::Four]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x35, &(*state)[KeyboardButton::Five]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x36, &(*state)[KeyboardButton::Six]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x37, &(*state)[KeyboardButton::Seven]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x38, &(*state)[KeyboardButton::Eight]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x39, &(*state)[KeyboardButton::Nine]);

	WindowsInputSystemLogic::UpdateWindowsButton(0x41, &(*state)[KeyboardButton::A]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x42, &(*state)[KeyboardButton::B]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x43, &(*state)[KeyboardButton::C]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x44, &(*state)[KeyboardButton::D]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x45, &(*state)[KeyboardButton::E]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x46, &(*state)[KeyboardButton::F]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x47, &(*state)[KeyboardButton::G]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x48, &(*state)[KeyboardButton::H]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x49, &(*state)[KeyboardButton::I]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4A, &(*state)[KeyboardButton::J]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4B, &(*state)[KeyboardButton::K]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4C, &(*state)[KeyboardButton::L]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4D, &(*state)[KeyboardButton::M]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4E, &(*state)[KeyboardButton::N]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x4F, &(*state)[KeyboardButton::O]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x50, &(*state)[KeyboardButton::P]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x51, &(*state)[KeyboardButton::Q]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x52, &(*state)[KeyboardButton::R]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x53, &(*state)[KeyboardButton::S]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x54, &(*state)[KeyboardButton::T]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x55, &(*state)[KeyboardButton::U]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x56, &(*state)[KeyboardButton::V]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x57, &(*state)[KeyboardButton::W]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x58, &(*state)[KeyboardButton::X]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x59, &(*state)[KeyboardButton::Y]);
	WindowsInputSystemLogic::UpdateWindowsButton(0x5A, &(*state)[KeyboardButton::Z]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_LWIN, &(*state)[KeyboardButton::LeftWindows]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RWIN, &(*state)[KeyboardButton::RightWindows]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_APPS, &(*state)[KeyboardButton::Applications]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SLEEP, &(*state)[KeyboardButton::Sleep]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD0, &(*state)[KeyboardButton::NumpadZero]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD1, &(*state)[KeyboardButton::NumpadOne]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD2, &(*state)[KeyboardButton::NumpadTwo]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD3, &(*state)[KeyboardButton::NumpadThree]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD4, &(*state)[KeyboardButton::NumpadFour]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD5, &(*state)[KeyboardButton::NumpadFive]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD6, &(*state)[KeyboardButton::NumpadSix]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD7, &(*state)[KeyboardButton::NumpadSeven]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD8, &(*state)[KeyboardButton::NumpadEight]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMPAD9, &(*state)[KeyboardButton::NumpadNine]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_MULTIPLY, &(*state)[KeyboardButton::NumpadMultiply]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_ADD, &(*state)[KeyboardButton::NumpadAdd]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SEPARATOR, &(*state)[KeyboardButton::NumpadSeparator]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SUBTRACT, &(*state)[KeyboardButton::NumpadSubtract]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_DECIMAL, &(*state)[KeyboardButton::NumpadDecimal]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_DIVIDE, &(*state)[KeyboardButton::NumpadDivide]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_F1, &(*state)[KeyboardButton::F1]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F2, &(*state)[KeyboardButton::F2]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F3, &(*state)[KeyboardButton::F3]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F4, &(*state)[KeyboardButton::F4]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F5, &(*state)[KeyboardButton::F5]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F6, &(*state)[KeyboardButton::F6]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F7, &(*state)[KeyboardButton::F7]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F8, &(*state)[KeyboardButton::F8]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F9, &(*state)[KeyboardButton::F9]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F10, &(*state)[KeyboardButton::F10]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F11, &(*state)[KeyboardButton::F11]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F12, &(*state)[KeyboardButton::F12]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F13, &(*state)[KeyboardButton::F13]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F14, &(*state)[KeyboardButton::F14]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F15, &(*state)[KeyboardButton::F15]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F16, &(*state)[KeyboardButton::F16]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F17, &(*state)[KeyboardButton::F17]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F18, &(*state)[KeyboardButton::F18]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F19, &(*state)[KeyboardButton::F19]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F20, &(*state)[KeyboardButton::F20]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F21, &(*state)[KeyboardButton::F21]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F22, &(*state)[KeyboardButton::F22]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F23, &(*state)[KeyboardButton::F23]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_F24, &(*state)[KeyboardButton::F24]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_NUMLOCK, &(*state)[KeyboardButton::NumLock]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_SCROLL, &(*state)[KeyboardButton::ScrollLock]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_LSHIFT, &(*state)[KeyboardButton::LeftShift]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RSHIFT, &(*state)[KeyboardButton::RightShift]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_LCONTROL, &(*state)[KeyboardButton::LeftControl]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RCONTROL, &(*state)[KeyboardButton::RightControl]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_LMENU, &(*state)[KeyboardButton::LeftAlt]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RMENU, &(*state)[KeyboardButton::RightAlt]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_BACK, &(*state)[KeyboardButton::BrowserBack]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_FORWARD, &(*state)[KeyboardButton::BrowserForward]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_REFRESH, &(*state)[KeyboardButton::BrowserRefresh]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_STOP, &(*state)[KeyboardButton::BrowserStop]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_SEARCH, &(*state)[KeyboardButton::BrowserSearch]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_FAVORITES, &(*state)[KeyboardButton::BrowserFavorites]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_BROWSER_HOME, &(*state)[KeyboardButton::BrowserStartAndHome]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_VOLUME_MUTE, &(*state)[KeyboardButton::VolumeMute]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_VOLUME_DOWN, &(*state)[KeyboardButton::VolumeDown]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_VOLUME_UP, &(*state)[KeyboardButton::VolumeUp]);

	WindowsInputSystemLogic::UpdateWindowsButton(VK_MEDIA_NEXT_TRACK, &(*state)[KeyboardButton::NextTrack]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_MEDIA_PREV_TRACK, &(*state)[KeyboardButton::PreviousTrack]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_MEDIA_STOP, &(*state)[KeyboardButton::StopMedia]);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_MEDIA_PLAY_PAUSE, &(*state)[KeyboardButton::PlayPause]);
}

/*
*	Updates the mouse state.
*/
void InputSystem::UpdateMouseState() NOEXCEPT
{
	//No need to update if the window isn't in focus.
	if (!CatalystPlatform::IsWindowInFocus())
	{
		return;
	}

	//Cache the state.
	MouseState *const RESTRICT state{ &_InputState._MouseState };

	//Copy the previous X and Y positions.
	state->_PreviousX = state->_CurrentX;
	state->_PreviousY = state->_CurrentY;

	//Get the current cursor position.
	POINT point;

	if (GetCursorPos(&point))
	{
		if (ScreenToClient(CatalystPlatform::_Window, &point))
		{
			RECT rectangle;

			if (GetClientRect(CatalystPlatform::_Window, &rectangle))
			{
				state->_CurrentX = CatalystBaseMath::Clamp<float>(static_cast<float>(point.x) / static_cast<float>(rectangle.right - rectangle.left), 0.0f, 1.0f);
				state->_CurrentY = CatalystBaseMath::Clamp<float>(1.0f - static_cast<float>(point.y) / static_cast<float>(rectangle.bottom - rectangle.top), 0.0f, 1.0f);

				state->_DeltaX = state->_CurrentX - state->_PreviousX;
				state->_DeltaY = state->_CurrentY - state->_PreviousY;
			}

			else
			{
				ASSERT(false, "Could not retrieve window rectangle.");
			}
		}

		else
		{
			ASSERT(false, "Could not convert cursor position to window coordinates.");
		}
	}

	else
	{
		ASSERT(false, "Could not retrieve the current cursor position.");
	}

	//Remember the last known cursor position.
	if (CatalystPlatform::IsCursorShown())
	{
		WindowsInputSystemData::_LastKnownCursorPosition._X = state->_CurrentX;
		WindowsInputSystemData::_LastKnownCursorPosition._Y = state->_CurrentY;
	}

	//Else, just imagine that the cursor is always at the last known cursor position.
	else
	{
		/*
		state->_PreviousX = state->_CurrentX = CatalystWindowsData::_LastKnownCursorPosition._X;
		state->_PreviousY = state->_CurrentY = CatalystWindowsData::_LastKnownCursorPosition._Y;

		//Get the current screen rectangle.
		RECT rectangle;

		if (GetClientRect(_Window, &rectangle))
		{
		//Calculate the last known cursor point.
		POINT last_known_cursor_point;

		last_known_cursor_point.x = static_cast<LONG>(CatalystBaseMath::LinearlyInterpolate(static_cast<float>(rectangle.left), static_cast<float>(rectangle.right), CatalystWindowsData::_LastKnownCursorPosition._X));
		last_known_cursor_point.y = static_cast<LONG>(CatalystBaseMath::LinearlyInterpolate(static_cast<float>(rectangle.bottom), static_cast<float>(rectangle.top), CatalystWindowsData::_LastKnownCursorPosition._Y));

		if (ClientToScreen(_Window, &last_known_cursor_point))
		{
		SetCursorPos(last_known_cursor_point.x, last_known_cursor_point.y);
		}

		else
		{
		ASSERT(false, "Could not convert cursor position to window coordinates.");
		}
		}

		else
		{
		ASSERT(false, "Could not retrieve window rectangle.");
		}
		*/

		//Imagine that the cursor is always at the center of the screen.
		state->_CurrentX = state->_CurrentY = state->_PreviousX = state->_PreviousY = 0.5f;

		//Get the current cursor position.
		POINT point;

		if (GetCursorPos(&point))
		{
			if (ScreenToClient(CatalystPlatform::_Window, &point))
			{
				RECT rectangle;

				if (GetClientRect(CatalystPlatform::_Window, &rectangle))
				{
					//Calculate the center and always place the cursor at the center of the screen.
					POINT center;
					POINT adjusted_center;

					center.x = adjusted_center.x = rectangle.left + ((rectangle.right - rectangle.left) / 2);
					center.y = adjusted_center.y = rectangle.top + ((rectangle.bottom - rectangle.top) / 2);

					if (ClientToScreen(CatalystPlatform::_Window, &adjusted_center))
					{
						SetCursorPos(adjusted_center.x, adjusted_center.y);

						const float current_x{ CatalystBaseMath::Clamp<float>(static_cast<float>(point.x) / static_cast<float>(rectangle.right - rectangle.left), 0.0f, 1.0f) };
						const float current_y{ CatalystBaseMath::Clamp<float>(1.0f - static_cast<float>(point.y) / static_cast<float>(rectangle.bottom - rectangle.top), 0.0f, 1.0f) };

						//Don't bother moving the cursor if it hasn't moved N pixels to account for integer rounding errors.
						state->_DeltaX = (current_x - 0.5f) * static_cast<float>(CatalystBaseMath::Absolute<int32>(point.x - center.x) > 0);
						state->_DeltaY = (current_y - 0.5f) * static_cast<float>(CatalystBaseMath::Absolute<int32>(point.y - center.y) > 0);
					}

					else
					{
						ASSERT(false, "Could not convert cursor position to window coordinates.");
					}
				}

				else
				{
					ASSERT(false, "Could not retrieve window rectangle.");
				}
			}

			else
			{
				ASSERT(false, "Could not convert cursor position to window coordinates.");
			}
		}

		else
		{
			ASSERT(false, "Could not retrieve the current cursor position.");
		}
	}

	//Update the button states.
	WindowsInputSystemLogic::UpdateWindowsButton(VK_LBUTTON, &state->_Left);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_MBUTTON, &state->_ScrollWheel);
	WindowsInputSystemLogic::UpdateWindowsButton(VK_RBUTTON, &state->_Right);

	//Update the scroll wheel step.
	state->_ScrollWheelStep = CatalystPlatform::GetAndResetScrollWheelStep();
}
#endif