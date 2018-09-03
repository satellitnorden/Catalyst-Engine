#if defined(CATALYST_WINDOWS)
//Header file.
#include <Platform/Windows/CatalystWindows.h>

//Core.
#include <Core/Core/CatalystCore.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>

//Math.
#include <Math/CatalystMath.h>

//Systems.
#include <Systems/EngineSystem.h>

//Windows.
#include <tchar.h>

//Static variable definitions.
HINSTANCE CatalystPlatform::instance;
int32 CatalystPlatform::showCommand;
HWND CatalystPlatform::window;

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
			EngineSystem::Instance->Terminate();

			return DefWindowProc(window, message, wParam, lParam);
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
void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, ButtonState &ButtonState) NOEXCEPT
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
	*	Updates a single keyboard button.
	*/
void UpdateKeyboardButton(const uint16 button, ButtonState &ButtonState) NOEXCEPT
{
	//The keyboard button is not pressed.
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

	//The keyboard button is pressed.
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
	windowInfo.hInstance = CatalystPlatform::instance;
	windowInfo.hIcon = LoadIcon(CatalystPlatform::instance, IDI_APPLICATION);
	windowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowInfo.hbrBackground = 0;
	windowInfo.lpszMenuName = NULL;
	windowInfo.lpszClassName = _T("Catalyst Engine");
	windowInfo.hIconSm = LoadIcon(windowInfo.hInstance, IDI_APPLICATION);

#if !defined(CATALYST_FINAL)
	if (!RegisterClassEx(&windowInfo))
	{
		BREAKPOINT();
	}
#else
	RegisterClassEx(&windowInfo);
#endif

	//Create the window.
	window = CreateWindow(	windowInfo.lpszClassName,
							_T(EngineSystem::Instance->GetProjectConfiguration().generalConfiguration.projectName.CString()),
							WS_MAXIMIZE | WS_SYSMENU,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							EngineSystem::Instance->GetProjectConfiguration().renderingConfiguration.resolution.width,
							EngineSystem::Instance->GetProjectConfiguration().renderingConfiguration.resolution.height,
							nullptr,
							nullptr,
							instance,
							nullptr);

#if !defined(CATALYST_FINAL)
	if (!window)
	{
		const DWORD result{ GetLastError() };

		BREAKPOINT();
	}
#endif

	//Show the window.
	ShowWindow(window, showCommand);

	//Update the window.
	UpdateWindow(window);
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
void CatalystPlatform::PreUpdate() NOEXCEPT
{

}

/*
*	Post-updates the platform.
*/
void CatalystPlatform::PostUpdate() NOEXCEPT
{
	//Process messages.
	MSG message;

	while (PeekMessage(&message, window, 0, 0, PM_REMOVE) > 0)
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
		state->isConnected = false;

		return;
	}

	else
	{
		state->isConnected = true;
	}

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, state->dpadUpButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, state->dpadDownButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, state->dpadLeftButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, state->dpadRightButtonState);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, state->startButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, state->backButtonState);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, state->leftThumbButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, state->rightThumbButtonState);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, state->leftShoulderButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, state->rightShoulderButtonState);

	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, state->aButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, state->bButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, state->xButtonState);
	UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, state->yButtonState);

	state->leftTriggerValue = static_cast<float>(xInputState.Gamepad.bLeftTrigger) / 255;
	state->rightTriggerValue = static_cast<float>(xInputState.Gamepad.bRightTrigger) / 255;

	state->leftThumbstickXValue = static_cast<float>(xInputState.Gamepad.sThumbLX) / 32'768;
	state->leftThumbstickYValue = static_cast<float>(xInputState.Gamepad.sThumbLY) / 32'768;

	state->rightThumbstickXValue = static_cast<float>(xInputState.Gamepad.sThumbRX) / 32'768;
	state->rightThumbstickYValue = static_cast<float>(xInputState.Gamepad.sThumbRY) / 32'768;

	//Calculate if the thumbstick values are in the deadzone.If so, reset them.
	if (CatalystMath::Absolute(state->leftThumbstickXValue) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->leftThumbstickXValue = 0.0f;
	}
		
	if (CatalystMath::Absolute(state->leftThumbstickYValue) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->leftThumbstickYValue = 0.0f;
	}

	if (CatalystMath::Absolute(state->rightThumbstickXValue) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->rightThumbstickXValue = 0.0f;
	}

	if (CatalystMath::Absolute(state->rightThumbstickYValue) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->rightThumbstickYValue = 0.0f;
	}
}

/*
*	Retrieves the current keyboard state.
*/
void CatalystPlatform::GetCurrentKeyboardState(KeyboardState *const RESTRICT state) NOEXCEPT
{
	UpdateKeyboardButton(VK_BACK, (*state)[KeyboardButton::Backspace]);
	UpdateKeyboardButton(VK_TAB, (*state)[KeyboardButton::Tab]);
	UpdateKeyboardButton(VK_CLEAR, (*state)[KeyboardButton::Clear]);
	UpdateKeyboardButton(VK_RETURN, (*state)[KeyboardButton::Enter]);
	UpdateKeyboardButton(VK_PAUSE, (*state)[KeyboardButton::Pause]);
	UpdateKeyboardButton(VK_CAPITAL, (*state)[KeyboardButton::CapsLock]);
	UpdateKeyboardButton(VK_ESCAPE, (*state)[KeyboardButton::Escape]);
	UpdateKeyboardButton(VK_SPACE, (*state)[KeyboardButton::Spacebar]);
	UpdateKeyboardButton(VK_PRIOR, (*state)[KeyboardButton::PageUp]);
	UpdateKeyboardButton(VK_NEXT, (*state)[KeyboardButton::PageDown]);
	UpdateKeyboardButton(VK_HOME, (*state)[KeyboardButton::Home]);
	UpdateKeyboardButton(VK_LEFT, (*state)[KeyboardButton::LeftArrow]);
	UpdateKeyboardButton(VK_UP, (*state)[KeyboardButton::UpArrow]);
	UpdateKeyboardButton(VK_RIGHT, (*state)[KeyboardButton::RightArrow]);
	UpdateKeyboardButton(VK_DOWN, (*state)[KeyboardButton::DownArrow]);
	UpdateKeyboardButton(VK_SELECT, (*state)[KeyboardButton::Select]);
	UpdateKeyboardButton(VK_PRINT, (*state)[KeyboardButton::Print]);
	UpdateKeyboardButton(VK_EXECUTE, (*state)[KeyboardButton::Execute]);
	UpdateKeyboardButton(VK_SNAPSHOT, (*state)[KeyboardButton::PrintScreen]);
	UpdateKeyboardButton(VK_INSERT, (*state)[KeyboardButton::Insert]);
	UpdateKeyboardButton(VK_DELETE, (*state)[KeyboardButton::Delete]);
	UpdateKeyboardButton(VK_HELP, (*state)[KeyboardButton::Help]);

	UpdateKeyboardButton(0x30, (*state)[KeyboardButton::Zero]);
	UpdateKeyboardButton(0x31, (*state)[KeyboardButton::One]);
	UpdateKeyboardButton(0x32, (*state)[KeyboardButton::Two]);
	UpdateKeyboardButton(0x33, (*state)[KeyboardButton::Three]);
	UpdateKeyboardButton(0x34, (*state)[KeyboardButton::Four]);
	UpdateKeyboardButton(0x35, (*state)[KeyboardButton::Five]);
	UpdateKeyboardButton(0x36, (*state)[KeyboardButton::Six]);
	UpdateKeyboardButton(0x37, (*state)[KeyboardButton::Seven]);
	UpdateKeyboardButton(0x38, (*state)[KeyboardButton::Eight]);
	UpdateKeyboardButton(0x39, (*state)[KeyboardButton::Nine]);

	UpdateKeyboardButton(0x41, (*state)[KeyboardButton::A]);
	UpdateKeyboardButton(0x42, (*state)[KeyboardButton::B]);
	UpdateKeyboardButton(0x43, (*state)[KeyboardButton::C]);
	UpdateKeyboardButton(0x44, (*state)[KeyboardButton::D]);
	UpdateKeyboardButton(0x45, (*state)[KeyboardButton::E]);
	UpdateKeyboardButton(0x46, (*state)[KeyboardButton::F]);
	UpdateKeyboardButton(0x47, (*state)[KeyboardButton::G]);
	UpdateKeyboardButton(0x48, (*state)[KeyboardButton::H]);
	UpdateKeyboardButton(0x49, (*state)[KeyboardButton::I]);
	UpdateKeyboardButton(0x4A, (*state)[KeyboardButton::J]);
	UpdateKeyboardButton(0x4B, (*state)[KeyboardButton::K]);
	UpdateKeyboardButton(0x4C, (*state)[KeyboardButton::L]);
	UpdateKeyboardButton(0x4D, (*state)[KeyboardButton::M]);
	UpdateKeyboardButton(0x4E, (*state)[KeyboardButton::N]);
	UpdateKeyboardButton(0x4F, (*state)[KeyboardButton::O]);
	UpdateKeyboardButton(0x50, (*state)[KeyboardButton::P]);
	UpdateKeyboardButton(0x51, (*state)[KeyboardButton::Q]);
	UpdateKeyboardButton(0x52, (*state)[KeyboardButton::R]);
	UpdateKeyboardButton(0x53, (*state)[KeyboardButton::S]);
	UpdateKeyboardButton(0x54, (*state)[KeyboardButton::T]);
	UpdateKeyboardButton(0x55, (*state)[KeyboardButton::U]);
	UpdateKeyboardButton(0x56, (*state)[KeyboardButton::V]);
	UpdateKeyboardButton(0x57, (*state)[KeyboardButton::W]);
	UpdateKeyboardButton(0x58, (*state)[KeyboardButton::X]);
	UpdateKeyboardButton(0x59, (*state)[KeyboardButton::Y]);
	UpdateKeyboardButton(0x5A, (*state)[KeyboardButton::Z]);

	UpdateKeyboardButton(VK_LWIN, (*state)[KeyboardButton::LeftWindows]);
	UpdateKeyboardButton(VK_RWIN, (*state)[KeyboardButton::RightWindows]);
	UpdateKeyboardButton(VK_APPS, (*state)[KeyboardButton::Applications]);
	UpdateKeyboardButton(VK_SLEEP, (*state)[KeyboardButton::Sleep]);

	UpdateKeyboardButton(VK_NUMPAD0, (*state)[KeyboardButton::NumpadZero]);
	UpdateKeyboardButton(VK_NUMPAD1, (*state)[KeyboardButton::NumpadOne]);
	UpdateKeyboardButton(VK_NUMPAD2, (*state)[KeyboardButton::NumpadTwo]);
	UpdateKeyboardButton(VK_NUMPAD3, (*state)[KeyboardButton::NumpadThree]);
	UpdateKeyboardButton(VK_NUMPAD4, (*state)[KeyboardButton::NumpadFour]);
	UpdateKeyboardButton(VK_NUMPAD5, (*state)[KeyboardButton::NumpadFive]);
	UpdateKeyboardButton(VK_NUMPAD6, (*state)[KeyboardButton::NumpadSix]);
	UpdateKeyboardButton(VK_NUMPAD7, (*state)[KeyboardButton::NumpadSeven]);
	UpdateKeyboardButton(VK_NUMPAD8, (*state)[KeyboardButton::NumpadEight]);
	UpdateKeyboardButton(VK_NUMPAD9, (*state)[KeyboardButton::NumpadNine]);

	UpdateKeyboardButton(VK_MULTIPLY, (*state)[KeyboardButton::NumpadMultiply]);
	UpdateKeyboardButton(VK_ADD, (*state)[KeyboardButton::NumpadAdd]);
	UpdateKeyboardButton(VK_SEPARATOR, (*state)[KeyboardButton::NumpadSeparator]);
	UpdateKeyboardButton(VK_SUBTRACT, (*state)[KeyboardButton::NumpadSubtract]);
	UpdateKeyboardButton(VK_DECIMAL, (*state)[KeyboardButton::NumpadDecimal]);
	UpdateKeyboardButton(VK_DIVIDE, (*state)[KeyboardButton::NumpadDivide]);

	UpdateKeyboardButton(VK_F1, (*state)[KeyboardButton::F1]);
	UpdateKeyboardButton(VK_F2, (*state)[KeyboardButton::F2]);
	UpdateKeyboardButton(VK_F3, (*state)[KeyboardButton::F3]);
	UpdateKeyboardButton(VK_F4, (*state)[KeyboardButton::F4]);
	UpdateKeyboardButton(VK_F5, (*state)[KeyboardButton::F5]);
	UpdateKeyboardButton(VK_F6, (*state)[KeyboardButton::F6]);
	UpdateKeyboardButton(VK_F7, (*state)[KeyboardButton::F7]);
	UpdateKeyboardButton(VK_F8, (*state)[KeyboardButton::F8]);
	UpdateKeyboardButton(VK_F9, (*state)[KeyboardButton::F9]);
	UpdateKeyboardButton(VK_F10, (*state)[KeyboardButton::F10]);
	UpdateKeyboardButton(VK_F11, (*state)[KeyboardButton::F11]);
	UpdateKeyboardButton(VK_F12, (*state)[KeyboardButton::F12]);
	UpdateKeyboardButton(VK_F13, (*state)[KeyboardButton::F13]);
	UpdateKeyboardButton(VK_F14, (*state)[KeyboardButton::F14]);
	UpdateKeyboardButton(VK_F15, (*state)[KeyboardButton::F15]);
	UpdateKeyboardButton(VK_F16, (*state)[KeyboardButton::F16]);
	UpdateKeyboardButton(VK_F17, (*state)[KeyboardButton::F17]);
	UpdateKeyboardButton(VK_F18, (*state)[KeyboardButton::F18]);
	UpdateKeyboardButton(VK_F19, (*state)[KeyboardButton::F19]);
	UpdateKeyboardButton(VK_F20, (*state)[KeyboardButton::F20]);
	UpdateKeyboardButton(VK_F21, (*state)[KeyboardButton::F21]);
	UpdateKeyboardButton(VK_F22, (*state)[KeyboardButton::F22]);
	UpdateKeyboardButton(VK_F23, (*state)[KeyboardButton::F23]);
	UpdateKeyboardButton(VK_F24, (*state)[KeyboardButton::F24]);

	UpdateKeyboardButton(VK_NUMLOCK, (*state)[KeyboardButton::NumLock]);
	UpdateKeyboardButton(VK_SCROLL, (*state)[KeyboardButton::ScrollLock]);
	UpdateKeyboardButton(VK_LSHIFT, (*state)[KeyboardButton::LeftShift]);
	UpdateKeyboardButton(VK_RSHIFT, (*state)[KeyboardButton::RightShift]);
	UpdateKeyboardButton(VK_LCONTROL, (*state)[KeyboardButton::LeftControl]);
	UpdateKeyboardButton(VK_RCONTROL, (*state)[KeyboardButton::RightControl]);
	UpdateKeyboardButton(VK_LMENU, (*state)[KeyboardButton::LeftAlt]);
	UpdateKeyboardButton(VK_RMENU, (*state)[KeyboardButton::RightAlt]);

	UpdateKeyboardButton(VK_BROWSER_BACK, (*state)[KeyboardButton::BrowserBack]);
	UpdateKeyboardButton(VK_BROWSER_FORWARD, (*state)[KeyboardButton::BrowserForward]);
	UpdateKeyboardButton(VK_BROWSER_REFRESH, (*state)[KeyboardButton::BrowserRefresh]);
	UpdateKeyboardButton(VK_BROWSER_STOP, (*state)[KeyboardButton::BrowserStop]);
	UpdateKeyboardButton(VK_BROWSER_SEARCH, (*state)[KeyboardButton::BrowserSearch]);
	UpdateKeyboardButton(VK_BROWSER_FAVORITES, (*state)[KeyboardButton::BrowserFavorites]);
	UpdateKeyboardButton(VK_BROWSER_HOME, (*state)[KeyboardButton::BrowserStartAndHome]);

	UpdateKeyboardButton(VK_VOLUME_MUTE, (*state)[KeyboardButton::VolumeMute]);
	UpdateKeyboardButton(VK_VOLUME_DOWN, (*state)[KeyboardButton::VolumeDown]);
	UpdateKeyboardButton(VK_VOLUME_UP, (*state)[KeyboardButton::VolumeUp]);

	UpdateKeyboardButton(VK_MEDIA_NEXT_TRACK, (*state)[KeyboardButton::NextTrack]);
	UpdateKeyboardButton(VK_MEDIA_PREV_TRACK, (*state)[KeyboardButton::PreviousTrack]);
	UpdateKeyboardButton(VK_MEDIA_STOP, (*state)[KeyboardButton::StopMedia]);
	UpdateKeyboardButton(VK_MEDIA_PLAY_PAUSE, (*state)[KeyboardButton::PlayPause]);
}

/*
*	Retrieves the current touch state.
*/
void CatalystPlatform::GetCurrentTouchState(TouchState *const RESTRICT state) NOEXCEPT
{
	//Not implemented on Windows.
}

#if !defined(CATALYST_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT message) NOEXCEPT
{
	OutputDebugString(message);
}
#endif
#endif