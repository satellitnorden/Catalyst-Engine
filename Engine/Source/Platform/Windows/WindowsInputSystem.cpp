#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/InputSystem.h>

//Core.
#include <Core/General/Pair.h>

//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

//Windows.
#include <Xinput.h>

//Windows input system data.
namespace WindowsInputSystemData
{
	//The last known cursor position.
	Vector2<float32> _LastKnownCursorPosition{ 0.5f, 0.5f };
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
	if (BaseMath::Absolute(state->_LeftThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickX = 0.0f;
	}

	else
	{
		state->_LeftThumbstickX = BaseMath::Scale(state->_LeftThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_LeftThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickY = 0.0f;
	}

	else
	{
		state->_LeftThumbstickY = BaseMath::Scale(state->_LeftThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_RightThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickX = 0.0f;
	}

	else
	{
		state->_RightThumbstickX = BaseMath::Scale(state->_RightThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_RightThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickY = 0.0f;
	}

	else
	{
		state->_RightThumbstickY = BaseMath::Scale(state->_RightThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}
}

/*
*	Updates the keyboard state.
*/
void InputSystem::UpdateKeyboardState() NOEXCEPT
{
	//Define constants.
	constexpr Pair<uint64, KeyboardButton> MAPPINGS[]
	{
		Pair<uint64, KeyboardButton>(VK_BACK, KeyboardButton::Backspace),
		Pair<uint64, KeyboardButton>(VK_TAB, KeyboardButton::Tab),
		Pair<uint64, KeyboardButton>(VK_CLEAR, KeyboardButton::Clear),
		Pair<uint64, KeyboardButton>(VK_RETURN, KeyboardButton::Enter),
		Pair<uint64, KeyboardButton>(VK_SHIFT, KeyboardButton::LeftShift),
		Pair<uint64, KeyboardButton>(VK_SHIFT, KeyboardButton::RightShift),
		Pair<uint64, KeyboardButton>(VK_PAUSE, KeyboardButton::Pause),
		Pair<uint64, KeyboardButton>(VK_CAPITAL, KeyboardButton::CapsLock),
		Pair<uint64, KeyboardButton>(VK_ESCAPE, KeyboardButton::Escape),
		Pair<uint64, KeyboardButton>(VK_SPACE, KeyboardButton::Spacebar),
		Pair<uint64, KeyboardButton>(VK_PRIOR, KeyboardButton::PageUp),
		Pair<uint64, KeyboardButton>(VK_NEXT, KeyboardButton::PageDown),
		Pair<uint64, KeyboardButton>(VK_HOME, KeyboardButton::Home),
		Pair<uint64, KeyboardButton>(VK_LEFT, KeyboardButton::LeftArrow),
		Pair<uint64, KeyboardButton>(VK_UP, KeyboardButton::UpArrow),
		Pair<uint64, KeyboardButton>(VK_RIGHT, KeyboardButton::RightArrow),
		Pair<uint64, KeyboardButton>(VK_DOWN, KeyboardButton::DownArrow),
		Pair<uint64, KeyboardButton>(VK_SELECT, KeyboardButton::Select),
		Pair<uint64, KeyboardButton>(VK_PRINT, KeyboardButton::Print),
		Pair<uint64, KeyboardButton>(VK_EXECUTE, KeyboardButton::Execute),
		Pair<uint64, KeyboardButton>(VK_SNAPSHOT, KeyboardButton::PrintScreen),
		Pair<uint64, KeyboardButton>(VK_INSERT, KeyboardButton::Insert),
		Pair<uint64, KeyboardButton>(VK_DELETE, KeyboardButton::Delete),
		Pair<uint64, KeyboardButton>(VK_HELP, KeyboardButton::Help),

		Pair<uint64, KeyboardButton>(0x30, KeyboardButton::ZERO),
		Pair<uint64, KeyboardButton>(0x31, KeyboardButton::ONE),
		Pair<uint64, KeyboardButton>(0x32, KeyboardButton::TWO),
		Pair<uint64, KeyboardButton>(0x33, KeyboardButton::THREE),
		Pair<uint64, KeyboardButton>(0x34, KeyboardButton::FOUR),
		Pair<uint64, KeyboardButton>(0x35, KeyboardButton::FIVE),
		Pair<uint64, KeyboardButton>(0x36, KeyboardButton::SIX),
		Pair<uint64, KeyboardButton>(0x37, KeyboardButton::SEVEN),
		Pair<uint64, KeyboardButton>(0x38, KeyboardButton::EIGHT),
		Pair<uint64, KeyboardButton>(0x39, KeyboardButton::NINE),

		Pair<uint64, KeyboardButton>(0x41, KeyboardButton::A),
		Pair<uint64, KeyboardButton>(0x42, KeyboardButton::B),
		Pair<uint64, KeyboardButton>(0x43, KeyboardButton::C),
		Pair<uint64, KeyboardButton>(0x44, KeyboardButton::D),
		Pair<uint64, KeyboardButton>(0x45, KeyboardButton::E),
		Pair<uint64, KeyboardButton>(0x46, KeyboardButton::F),
		Pair<uint64, KeyboardButton>(0x47, KeyboardButton::G),
		Pair<uint64, KeyboardButton>(0x48, KeyboardButton::H),
		Pair<uint64, KeyboardButton>(0x49, KeyboardButton::I),
		Pair<uint64, KeyboardButton>(0x4A, KeyboardButton::J),
		Pair<uint64, KeyboardButton>(0x4B, KeyboardButton::K),
		Pair<uint64, KeyboardButton>(0x4C, KeyboardButton::L),
		Pair<uint64, KeyboardButton>(0x4D, KeyboardButton::M),
		Pair<uint64, KeyboardButton>(0x4E, KeyboardButton::N),
		Pair<uint64, KeyboardButton>(0x4F, KeyboardButton::O),
		Pair<uint64, KeyboardButton>(0x50, KeyboardButton::P),
		Pair<uint64, KeyboardButton>(0x51, KeyboardButton::Q),
		Pair<uint64, KeyboardButton>(0x52, KeyboardButton::R),
		Pair<uint64, KeyboardButton>(0x53, KeyboardButton::S),
		Pair<uint64, KeyboardButton>(0x54, KeyboardButton::T),
		Pair<uint64, KeyboardButton>(0x55, KeyboardButton::U),
		Pair<uint64, KeyboardButton>(0x56, KeyboardButton::V),
		Pair<uint64, KeyboardButton>(0x57, KeyboardButton::W),
		Pair<uint64, KeyboardButton>(0x58, KeyboardButton::X),
		Pair<uint64, KeyboardButton>(0x59, KeyboardButton::Y),
		Pair<uint64, KeyboardButton>(0x5A, KeyboardButton::Z),

		Pair<uint64, KeyboardButton>(VK_LWIN, KeyboardButton::LeftWindows),
		Pair<uint64, KeyboardButton>(VK_RWIN, KeyboardButton::RightWindows),
		Pair<uint64, KeyboardButton>(VK_APPS, KeyboardButton::Applications),
		Pair<uint64, KeyboardButton>(VK_SLEEP, KeyboardButton::Sleep),

		Pair<uint64, KeyboardButton>(VK_NUMPAD0, KeyboardButton::NumpadZero),
		Pair<uint64, KeyboardButton>(VK_NUMPAD1, KeyboardButton::NumpadOne),
		Pair<uint64, KeyboardButton>(VK_NUMPAD2, KeyboardButton::NumpadTwo),
		Pair<uint64, KeyboardButton>(VK_NUMPAD3, KeyboardButton::NumpadThree),
		Pair<uint64, KeyboardButton>(VK_NUMPAD4, KeyboardButton::NumpadFour),
		Pair<uint64, KeyboardButton>(VK_NUMPAD5, KeyboardButton::NumpadFive),
		Pair<uint64, KeyboardButton>(VK_NUMPAD6, KeyboardButton::NumpadSix),
		Pair<uint64, KeyboardButton>(VK_NUMPAD7, KeyboardButton::NumpadSeven),
		Pair<uint64, KeyboardButton>(VK_NUMPAD8, KeyboardButton::NumpadEight),
		Pair<uint64, KeyboardButton>(VK_NUMPAD9, KeyboardButton::NumpadNine),

		Pair<uint64, KeyboardButton>(VK_MULTIPLY, KeyboardButton::MULTIPLY),
		Pair<uint64, KeyboardButton>(VK_ADD, KeyboardButton::ADD),
		Pair<uint64, KeyboardButton>(VK_SEPARATOR, KeyboardButton::SEPARATOR),
		Pair<uint64, KeyboardButton>(VK_SUBTRACT, KeyboardButton::SUBTRACT),
		Pair<uint64, KeyboardButton>(VK_DECIMAL, KeyboardButton::DECIMAL),
		Pair<uint64, KeyboardButton>(VK_DIVIDE, KeyboardButton::DIVIDE),

		Pair<uint64, KeyboardButton>(VK_F1, KeyboardButton::F1),
		Pair<uint64, KeyboardButton>(VK_F2, KeyboardButton::F2),
		Pair<uint64, KeyboardButton>(VK_F3, KeyboardButton::F3),
		Pair<uint64, KeyboardButton>(VK_F4, KeyboardButton::F4),
		Pair<uint64, KeyboardButton>(VK_F5, KeyboardButton::F5),
		Pair<uint64, KeyboardButton>(VK_F6, KeyboardButton::F6),
		Pair<uint64, KeyboardButton>(VK_F7, KeyboardButton::F7),
		Pair<uint64, KeyboardButton>(VK_F8, KeyboardButton::F8),
		Pair<uint64, KeyboardButton>(VK_F9, KeyboardButton::F9),
		Pair<uint64, KeyboardButton>(VK_F10, KeyboardButton::F10),
		Pair<uint64, KeyboardButton>(VK_F11, KeyboardButton::F11),
		Pair<uint64, KeyboardButton>(VK_F12, KeyboardButton::F12),
		Pair<uint64, KeyboardButton>(VK_F13, KeyboardButton::F13),
		Pair<uint64, KeyboardButton>(VK_F14, KeyboardButton::F14),
		Pair<uint64, KeyboardButton>(VK_F15, KeyboardButton::F15),
		Pair<uint64, KeyboardButton>(VK_F16, KeyboardButton::F16),
		Pair<uint64, KeyboardButton>(VK_F17, KeyboardButton::F17),
		Pair<uint64, KeyboardButton>(VK_F18, KeyboardButton::F18),
		Pair<uint64, KeyboardButton>(VK_F19, KeyboardButton::F19),
		Pair<uint64, KeyboardButton>(VK_F20, KeyboardButton::F20),
		Pair<uint64, KeyboardButton>(VK_F21, KeyboardButton::F21),
		Pair<uint64, KeyboardButton>(VK_F22, KeyboardButton::F22),
		Pair<uint64, KeyboardButton>(VK_F23, KeyboardButton::F23),
		Pair<uint64, KeyboardButton>(VK_F24, KeyboardButton::F24),

		Pair<uint64, KeyboardButton>(VK_NUMLOCK, KeyboardButton::NumLock),
		Pair<uint64, KeyboardButton>(VK_SCROLL, KeyboardButton::ScrollLock),
		Pair<uint64, KeyboardButton>(VK_LCONTROL, KeyboardButton::LeftControl),
		Pair<uint64, KeyboardButton>(VK_RCONTROL, KeyboardButton::RightControl),
		Pair<uint64, KeyboardButton>(VK_LMENU, KeyboardButton::LeftAlt),
		Pair<uint64, KeyboardButton>(VK_RMENU, KeyboardButton::RightAlt),

		Pair<uint64, KeyboardButton>(VK_BROWSER_BACK, KeyboardButton::BrowserBack),
		Pair<uint64, KeyboardButton>(VK_BROWSER_FORWARD, KeyboardButton::BrowserForward),
		Pair<uint64, KeyboardButton>(VK_BROWSER_REFRESH, KeyboardButton::BrowserRefresh),
		Pair<uint64, KeyboardButton>(VK_BROWSER_STOP, KeyboardButton::BrowserStop),
		Pair<uint64, KeyboardButton>(VK_BROWSER_SEARCH, KeyboardButton::BrowserSearch),	
		Pair<uint64, KeyboardButton>(VK_BROWSER_FAVORITES, KeyboardButton::BrowserFavorites),
		Pair<uint64, KeyboardButton>(VK_BROWSER_HOME, KeyboardButton::BrowserStartAndHome),

		Pair<uint64, KeyboardButton>(VK_VOLUME_MUTE, KeyboardButton::VolumeMute),
		Pair<uint64, KeyboardButton>(VK_VOLUME_DOWN, KeyboardButton::VolumeDown),
		Pair<uint64, KeyboardButton>(VK_VOLUME_UP, KeyboardButton::VolumeUp),

		Pair<uint64, KeyboardButton>(VK_MEDIA_NEXT_TRACK, KeyboardButton::NextTrack),
		Pair<uint64, KeyboardButton>(VK_MEDIA_PREV_TRACK, KeyboardButton::PreviousTrack),
		Pair<uint64, KeyboardButton>(VK_MEDIA_STOP, KeyboardButton::StopMedia),
		Pair<uint64, KeyboardButton>(VK_MEDIA_PLAY_PAUSE, KeyboardButton::PlayPause),

		Pair<uint64, KeyboardButton>(VK_OEM_COMMA, KeyboardButton::COMMA),
		Pair<uint64, KeyboardButton>(VK_OEM_PERIOD, KeyboardButton::PERIOD)
	};

	//No need to update if the window isn't in focus.
	if (!CatalystPlatform::IsWindowInFocus())
	{
		return;
	}

	//Cache the state.
	KeyboardState *const RESTRICT state{ &_InputState._KeyboardState };

	//Update all keyboard buttons.
	for (uint64 i{ 0 }; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
	{
		switch ((*state)[static_cast<KeyboardButton>(i)])
		{
			case ButtonState::PRESSED:
			{
				(*state)[static_cast<KeyboardButton>(i)] = ButtonState::PRESSED_HELD;

				break;
			}

			case ButtonState::RELEASED:
			{
				(*state)[static_cast<KeyboardButton>(i)] = ButtonState::RELEASED_HELD;

				break;
			}
		}
	}

	//Process key down events.
	for (const uint64 key_down_event : CatalystPlatformWindows::_KeyDownEvents)
	{
		for (uint64 i{ 0 }; i < ARRAY_LENGTH(MAPPINGS); ++i)
		{
			if (key_down_event == MAPPINGS[i]._First)
			{
				(*state)[MAPPINGS[i]._Second] = ButtonState::PRESSED;

				break;
			}
		}
	}

	//Process key up events.
	for (const uint64 key_up_event : CatalystPlatformWindows::_KeyUpEvents)
	{
		for (uint64 i{ 0 }; i < ARRAY_LENGTH(MAPPINGS); ++i)
		{
			if (key_up_event == MAPPINGS[i]._First)
			{
				(*state)[MAPPINGS[i]._Second] = ButtonState::RELEASED;

				break;
			}
		}
	}
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
		if (ScreenToClient(CatalystPlatformWindows::_Window, &point))
		{
			RECT rectangle;

			if (GetClientRect(CatalystPlatformWindows::_Window, &rectangle))
			{
				state->_CurrentX = BaseMath::Clamp<float>(static_cast<float>(point.x) / static_cast<float>(rectangle.right - rectangle.left), 0.0f, 1.0f);
				state->_CurrentY = BaseMath::Clamp<float>(1.0f - static_cast<float>(point.y) / static_cast<float>(rectangle.bottom - rectangle.top), 0.0f, 1.0f);

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

		last_known_cursor_point.x = static_cast<LONG>(BaseMath::LinearlyInterpolate(static_cast<float>(rectangle.left), static_cast<float>(rectangle.right), CatalystWindowsData::_LastKnownCursorPosition._X));
		last_known_cursor_point.y = static_cast<LONG>(BaseMath::LinearlyInterpolate(static_cast<float>(rectangle.bottom), static_cast<float>(rectangle.top), CatalystWindowsData::_LastKnownCursorPosition._Y));

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
			if (ScreenToClient(CatalystPlatformWindows::_Window, &point))
			{
				RECT rectangle;

				if (GetClientRect(CatalystPlatformWindows::_Window, &rectangle))
				{
					//Calculate the center and always place the cursor at the center of the screen.
					POINT center;
					POINT adjusted_center;

					center.x = adjusted_center.x = rectangle.left + ((rectangle.right - rectangle.left) / 2);
					center.y = adjusted_center.y = rectangle.top + ((rectangle.bottom - rectangle.top) / 2);

					if (ClientToScreen(CatalystPlatformWindows::_Window, &adjusted_center))
					{
						SetCursorPos(adjusted_center.x, adjusted_center.y);

						const float current_x{ BaseMath::Clamp<float>(static_cast<float>(point.x) / static_cast<float>(rectangle.right - rectangle.left), 0.0f, 1.0f) };
						const float current_y{ BaseMath::Clamp<float>(1.0f - static_cast<float>(point.y) / static_cast<float>(rectangle.bottom - rectangle.top), 0.0f, 1.0f) };

						//Don't bother moving the cursor if it hasn't moved N pixels to account for integer rounding errors.
						state->_DeltaX = (current_x - 0.5f) * static_cast<float>(BaseMath::Absolute<int32>(point.x - center.x) > 0);
						state->_DeltaY = (current_y - 0.5f) * static_cast<float>(BaseMath::Absolute<int32>(point.y - center.y) > 0);
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

/*
*	Updates the touch state.
*/
void InputSystem::UpdateTouchState() NOEXCEPT
{
	//Windows doesn't have touch. I think. (:
}
#endif