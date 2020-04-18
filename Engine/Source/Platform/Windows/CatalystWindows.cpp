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
#include <Math/General/Vector.h>

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
	//The last known cursor position.
	Vector2<float> _LastKnownCursorPosition{ 0.5f, 0.5f };

	//Denotes whether or not the window is in focus.
	bool _IsWindowInFocus{ true };

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
			CatalystEngineSystem::Instance->SetShouldTerminate();

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
*	Sets the visibility of the cursor.
*/
FORCE_INLINE void SetCursorVisibility(const bool visibility) NOEXCEPT
{
	if (CatalystWindowsData::_CursorShown != visibility)
	{
		CatalystWindowsData::_CursorShown = visibility;

		if (visibility)
		{
			while (ShowCursor(true) < 0);
		}

		else
		{
			while (ShowCursor(false) > 0);
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
	windowInfo.hInstance = CatalystPlatform::_Instance;
	windowInfo.hIcon = LoadIcon(CatalystPlatform::_Instance, IDI_APPLICATION);
	windowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowInfo.hbrBackground = 0;
	windowInfo.lpszMenuName = NULL;
	windowInfo.lpszClassName = _T("Catalyst Engine");
	windowInfo.hIconSm = LoadIcon(windowInfo.hInstance, IDI_APPLICATION);

#if !defined(CATALYST_CONFIGURATION_FINAL)
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
							WS_MAXIMIZE | WS_SYSMENU | WS_EX_TRANSPARENT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Width,
							CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Height,
							nullptr,
							nullptr,
							_Instance,
							nullptr);

#if !defined(CATALYST_CONFIGURATION_FINAL)
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
*	Pre-updates the platform.
*/
void CatalystPlatform::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Determine if the window is in focus.
	CatalystWindowsData::_IsWindowInFocus = GetFocus() == _Window;
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
*	Retrieves the current mouse state.
*/
void CatalystPlatform::GetCurrentMouseState(MouseState *const RESTRICT state) NOEXCEPT
{
	//No need to update if the window isn't in focus.
	if (!CatalystWindowsData::_IsWindowInFocus)
	{
		return;
	}

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

	//Remember the last known cursor position.
	if (CatalystWindowsData::_CursorShown)
	{
		CatalystWindowsData::_LastKnownCursorPosition._X = state->_CurrentX;
		CatalystWindowsData::_LastKnownCursorPosition._Y = state->_CurrentY;
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
*	Returns whether or not the window is in focus.
*/
bool CatalystPlatform::IsWindowInFocus() NOEXCEPT
{
	return CatalystWindowsData::_IsWindowInFocus;
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