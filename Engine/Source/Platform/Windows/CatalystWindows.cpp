#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Platform/Windows/CatalystWindows.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Windows.
#include <tchar.h>
#include <Xinput.h>

//Static variable definitions.
HINSTANCE CatalystPlatform::_Instance;
int32 CatalystPlatform::_ShowCommand;
HWND CatalystPlatform::_Window;

//Catalyst Windows data.
namespace CatalystWindowsData
{
	//Denotes whether or not the cursor is shown.
	bool _CursorShown{ true };

	//The scroll wheel step.
	int8 _ScrollWheelStep{ 0 };
}

/*
*	Window procedure handling.
*/
LRESULT CALLBACK WindowProcedure(	_In_ HWND   window,
									_In_ UINT   message,
									_In_ WPARAM wParam,
									_In_ LPARAM lParam) NOEXCEPT
{
	switch (message)
	{
		case WM_DESTROY:
		{
			CatalystEngineSystem::Instance->SetShouldTerminate(true);

			return DefWindowProc(window, message, wParam, lParam);
		}

		case WM_MOUSEWHEEL:
		{
			CatalystWindowsData::_ScrollWheelStep = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

			return 0;
		}

		default:
		{
			return DefWindowProc(window, message, wParam, lParam);
		}
	}
}

/*
*	Updates a single gamepad button.
*/
FORCE_INLINE void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, ButtonState &ButtonState) NOEXCEPT
{
	//The gamepad button is not pressed.
	if (!(wButtons & xInputButton))
	{
		if (ButtonState == ButtonState::ReleasedHold)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::PressedHold || ButtonState == ButtonState::Pressed)
		{
			ButtonState = ButtonState::Released;
		}

		else if (ButtonState == ButtonState::Released)
		{
			ButtonState = ButtonState::ReleasedHold;
		}	
	}

	//The gamepad button is pressed.
	else
	{
		if (ButtonState == ButtonState::PressedHold)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::ReleasedHold || ButtonState == ButtonState::Released)
		{
			ButtonState = ButtonState::Pressed;
		}

		else if (ButtonState == ButtonState::Pressed)
		{
			ButtonState = ButtonState::PressedHold;
		}
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
		if (ButtonState == ButtonState::ReleasedHold)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::PressedHold || ButtonState == ButtonState::Pressed)
		{
			ButtonState = ButtonState::Released;
		}

		else if (ButtonState == ButtonState::Released)
		{
			ButtonState = ButtonState::ReleasedHold;
		}
	}

	//The Windows button is pressed.
	else
	{
		if (ButtonState == ButtonState::PressedHold)
		{
			//Do nothing. This is here since this is the most common state, and it enables the forthcoming if statements to evade executing.
		}

		else if (ButtonState == ButtonState::ReleasedHold || ButtonState == ButtonState::Released)
		{
			ButtonState = ButtonState::Pressed;
		}

		else if (ButtonState == ButtonState::Pressed)
		{
			ButtonState = ButtonState::PressedHold;
		}
	}
}

/*
*	Sets the visibility of the cursor.
*/
FORCE_INLINE void SetCursorVisibility(const bool visibility) NOEXCEPT
{
	if (visibility)
	{
		ShowCursor(true);
	}

	else
	{
		while (ShowCursor(false) >= 0);
	}

	CatalystWindowsData::_CursorShown = visibility;
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Create the window info.
	WNDCLASSEX windowInfo;

	windowInfo.cbSize = sizeof(WNDCLASSEX);
	windowInfo.style = 0;
	windowInfo.lpfnWndProc = WindowProcedure;
	windowInfo.cbClsExtra = 0;
	windowInfo.cbWndExtra = 0;
	windowInfo.hInstance = CatalystPlatform::_Instance;
	windowInfo.hIcon = LoadIcon(CatalystPlatform::_Instance, IDI_APPLICATION);
	windowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowInfo.hbrBackground = 0;
	windowInfo.lpszMenuName = NULL;
	windowInfo.lpszClassName = _T("Catalyst Engine");
	windowInfo.hIconSm = LoadIcon(windowInfo.hInstance, IDI_APPLICATION);

#if defined(CATALYST_CONFIGURATION_DEBUG)
	if (!RegisterClassEx(&windowInfo))
	{
		BREAKPOINT();
	}
#else
	RegisterClassEx(&windowInfo);
#endif

	//Create the window.
	_Window = CreateWindow(	windowInfo.lpszClassName,
							_T(CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data()),
							WS_MAXIMIZE | WS_SYSMENU,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Width,
							CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Height,
							nullptr,
							nullptr,
							_Instance,
							nullptr);

#if defined(CATALYST_CONFIGURATION_DEBUG)
	if (!_Window)
	{
		const DWORD result{ GetLastError() };

		BREAKPOINT();
	}
#endif

	//Show the window.
	ShowWindow(_Window, _ShowCommand);

	//Update the window.
	UpdateWindow(_Window);
}

/*
*	Post-nitializes the platform.
*/
void CatalystPlatform::PostInitialize() NOEXCEPT
{

}

/*
*	Pre-updates the platform.
*/
void CatalystPlatform::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{

}

/*
*	Post-updates the platform.
*/
void CatalystPlatform::PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Process messages.
	MSG message;

	while (PeekMessage(&message, _Window, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

/*
*	Releases the platform.
*/
void CatalystPlatform::Release() NOEXCEPT
{

}

/*
*	Retrieves the current gamepad state.
*/
void CatalystPlatform::GetCurrentGamepadState(const uint8 index, GamepadState *const RESTRICT state) NOEXCEPT
{
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
*	Retrieves the current keyboard state.
*/
void CatalystPlatform::GetCurrentKeyboardState(KeyboardState *const RESTRICT state) NOEXCEPT
{
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
*	Retrieves the current mouse state.
*/
void CatalystPlatform::GetCurrentMouseState(MouseState *const RESTRICT state) NOEXCEPT
{
	if (CatalystWindowsData::_CursorShown)
	{
		//Copy the previous X and Y positions.
		state->_PreviousX = state->_CurrentX;
		state->_PreviousY = state->_CurrentY;

		//Get the current cursor position.
		POINT point;

		if (GetCursorPos(&point))
		{
			if (ScreenToClient(_Window, &point))
			{
				RECT rectangle;

				if (GetClientRect(_Window, &rectangle))
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
	}
	
	else
	{
		//Imagine that the cursor is always at the center of the screen.
		state->_CurrentX = state->_CurrentY = state->_PreviousX = state->_PreviousY = 0.5f;

		//Get the current cursor position.
		POINT point;

		if (GetCursorPos(&point))
		{
			if (ScreenToClient(_Window, &point))
			{
				RECT rectangle;

				if (GetClientRect(_Window, &rectangle))
				{
					//Calculate the center and always place the cursor at the center of the screen.
					POINT center;
					POINT adjusted_center;

					center.x = adjusted_center.x = rectangle.left + ((rectangle.right - rectangle.left) / 2);
					center.y = adjusted_center.y = rectangle.top + ((rectangle.bottom - rectangle.top) / 2);

					if (ClientToScreen(_Window, &adjusted_center))
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
	UpdateWindowsButton(VK_LBUTTON, state->_Left);
	UpdateWindowsButton(VK_MBUTTON, state->_ScrollWheel);
	UpdateWindowsButton(VK_RBUTTON, state->_Right);

	//Update the scroll wheel step.
	state->_ScrollWheelStep = CatalystWindowsData::_ScrollWheelStep;
	CatalystWindowsData::_ScrollWheelStep = 0;
}

/*
*	Hides the cursor.
*/
void CatalystPlatform::HideCursor() NOEXCEPT
{
	SetCursorVisibility(false);
}

/*
*	Shows the cursor.
*/
void CatalystPlatform::ShowCursor() NOEXCEPT
{
	SetCursorVisibility(true);
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT message) NOEXCEPT
{
	OutputDebugString(message);
}
#endif
#endif