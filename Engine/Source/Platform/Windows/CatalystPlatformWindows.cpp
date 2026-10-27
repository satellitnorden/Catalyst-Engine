#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Platform/Windows/CatalystPlatformWindows.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>

//Math.
#include <Math/General/Vector.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/LogSystem.h>

//Third party.
#include <ThirdParty/b_stacktrace/b_stacktrace.h>

//Windows.
#include <Lmcons.h>
#include <Xinput.h>

//STl.
#include <stdio.h>

//Static variable definitions.
HINSTANCE CatalystPlatformWindows::_Instance;
int32 CatalystPlatformWindows::_ShowCommand;
bool CatalystPlatformWindows::_WindowInfoRegistered{ false };
WNDCLASSEX CatalystPlatformWindows::_WindowInfo;
HWND CatalystPlatformWindows::_MainWindow;
DynamicArray<uint64> CatalystPlatformWindows::_KeyDownEvents;
DynamicArray<uint64> CatalystPlatformWindows::_KeyUpEvents;
DynamicArray<char> CatalystPlatformWindows::_InputCharacters;

//Catalyst Windows data.
namespace CatalystWindowsData
{
	//Denotes whether or not the window is in focus.
	bool _IsWindowInFocus{ true };

	//Denotes the cursor's requested visibility.
	bool _CursorRequestedVisibility{ true };

	//Denotes the cursor's current visibility.
	bool _CursorCurrentVisibility{ true };

	//The cached cursor position.
	POINT _CachedCursorPosition{ 0, 0 };

	//The scroll wheel step.
	int8 _ScrollWheelStep{ 0 };

}

//Catalyst Windows logic.
namespace CatalystWindowsLogic
{

	/*
	*	Updates the given platform window.
	*/
	void UpdatePlatformWindow(void *const RESTRICT window) NOEXCEPT
	{
		HWND _window{ static_cast<HWND>(window) };

		if (_window == CatalystPlatformWindows::_MainWindow)
		{
			//Determine if the window is in focus.
			CatalystWindowsData::_IsWindowInFocus = GetFocus() == _window;

			//Clear the keyboard events.
			CatalystPlatformWindows::_KeyDownEvents.Clear();
			CatalystPlatformWindows::_KeyUpEvents.Clear();
			CatalystPlatformWindows::_InputCharacters.Clear();
		}

		//Process messages.
		MSG message;

		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (_window == CatalystPlatformWindows::_MainWindow)
		{
			//Update the cursor.
			if (CatalystWindowsData::_CursorCurrentVisibility != CatalystWindowsData::_CursorRequestedVisibility)
			{
				//If the cursor is being hidden, remember the cursor position for when it is shown again.
				if (CatalystWindowsData::_CursorCurrentVisibility)
				{
					POINT cursor_position;

					if (GetCursorPos(&cursor_position))
					{
						CatalystWindowsData::_CachedCursorPosition = cursor_position;

						RECT rectangle;

						if (GetClientRect(_window, &rectangle))
						{
							POINT minimum{ rectangle.left, rectangle.bottom };
							POINT maximum{ rectangle.right, rectangle.top };

							if (ClientToScreen(_window, &minimum) && ClientToScreen(_window, &maximum))
							{
								SetCursorPos(minimum.x + ((maximum.x - minimum.x) / 2), minimum.y + ((maximum.y - minimum.y) / 2));
							}
						}
					}
				}

				else
				{
					SetCursorPos(CatalystWindowsData::_CachedCursorPosition.x, CatalystWindowsData::_CachedCursorPosition.y);
				}

				CatalystWindowsData::_CursorCurrentVisibility = CatalystWindowsData::_CursorRequestedVisibility;

				if (CatalystWindowsData::_CursorRequestedVisibility)
				{
					while (ShowCursor(true) < 0);
				}

				else
				{
					while (ShowCursor(false) >= 0);
				}
			}
		}
	}

}

/*
*	Window procedure handling.
*/
LRESULT CALLBACK WindowProcedure(	_In_ HWND   window,
									_In_ UINT   message,
									_In_ WPARAM wParam,
									_In_ LPARAM lParam) NOEXCEPT
{
	if (window != CatalystPlatformWindows::_MainWindow)
	{
		return DefWindowProc(window, message, wParam, lParam);
	}

	switch (message)
	{
		case WM_DESTROY:
		{
			CatalystEngineSystem::Instance->SetShouldTerminate();

			break;
		}

		case WM_MOUSEWHEEL:
		{
			CatalystWindowsData::_ScrollWheelStep = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

			break;
		}

		case WM_KEYDOWN:
		{
			CatalystPlatformWindows::_KeyDownEvents.Emplace(wParam);

			break;
		}

		case WM_KEYUP:
		{
			CatalystPlatformWindows::_KeyUpEvents.Emplace(wParam);

			break;
		}

		case WM_CHAR:
		{
			CatalystPlatformWindows::_InputCharacters.Emplace((char)(wParam & 255));

			break;
		}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

/*
*	Registers the window info.
*/
FORCE_INLINE void RegisterWindowInfo() NOEXCEPT
{
	if (CatalystPlatformWindows::_WindowInfoRegistered)
	{
		return;
	}

	CatalystPlatformWindows::_WindowInfo.cbSize = sizeof(WNDCLASSEX);
	CatalystPlatformWindows::_WindowInfo.style = 0;
	CatalystPlatformWindows::_WindowInfo.lpfnWndProc = WindowProcedure;
	CatalystPlatformWindows::_WindowInfo.cbClsExtra = 0;
	CatalystPlatformWindows::_WindowInfo.cbWndExtra = 0;
	CatalystPlatformWindows::_WindowInfo.hInstance = CatalystPlatformWindows::_Instance;
	CatalystPlatformWindows::_WindowInfo.hIcon = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);
	CatalystPlatformWindows::_WindowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	CatalystPlatformWindows::_WindowInfo.hbrBackground = 0;
	CatalystPlatformWindows::_WindowInfo.lpszMenuName = NULL;
	CatalystPlatformWindows::_WindowInfo.lpszClassName = _T("Catalyst Engine");
	CatalystPlatformWindows::_WindowInfo.hIconSm = LoadIcon(CatalystPlatformWindows::_WindowInfo.hInstance, IDI_APPLICATION);

#if !defined(CATALYST_CONFIGURATION_FINAL)
	if (!RegisterClassEx(&CatalystPlatformWindows::_WindowInfo))
	{
		BREAKPOINT();
	}
#else
	RegisterClassEx(&CatalystPlatformWindows::_WindowInfo);
#endif

	CatalystPlatformWindows::_WindowInfoRegistered = true;
}

/*
*	Sets the visibility of the cursor.
*/
FORCE_INLINE void SetCursorVisibility(const bool visibility) NOEXCEPT
{
	//Set the requested visibility.
	CatalystWindowsData::_CursorRequestedVisibility = visibility;
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	//Register the window info.
	RegisterWindowInfo();

	//Create the window.
	{
		const bool fullscreen{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._InitialFullScreen };

		char window_name_buffer[256];

		if (fullscreen)
		{
			sprintf_s(window_name_buffer, "%s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
		}

		else
		{
#if defined(CATALYST_EDITOR)
			sprintf_s(window_name_buffer, "Catalyst Editor - %s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#else
			sprintf_s(window_name_buffer, "%s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#endif
		}

		Resolution resolution;

		if (CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Valid())
		{
			resolution = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Get();
		}

		else
		{
			CatalystPlatform::GetDefaultResolution(&resolution._Width, &resolution._Height);
		}

		CatalystPlatformWindows::_MainWindow = static_cast<HWND>(CreatePlatformWindow(window_name_buffer, resolution._Width, resolution._Height, fullscreen ? WindowType::FULLSCREEN : WindowType::DEFAULT));
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	if (!CatalystPlatformWindows::_MainWindow)
	{
		const DWORD result{ GetLastError() };

		BREAKPOINT();
	}
#endif

	//Allocate an appropriate amount of memory for keyboard events.
	CatalystPlatformWindows::_KeyDownEvents.Reserve(32);
	CatalystPlatformWindows::_KeyUpEvents.Reserve(32);
	CatalystPlatformWindows::_InputCharacters.Reserve(32);
}

/*
*	Updates the platform before everything else.
*/
void CatalystPlatform::PlatformPreUpdate() NOEXCEPT
{

}

/*
*	Updates the platform after everything else.
*/
void CatalystPlatform::PlatformPostUpdate() NOEXCEPT
{
	CatalystWindowsLogic::UpdatePlatformWindow(CatalystPlatformWindows::_MainWindow);
}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{

}

/*
*	Returns if the given platform window is alive.
*/
NO_DISCARD bool CatalystPlatform::IsPlatformWindowAlive(void* const RESTRICT window) NOEXCEPT
{
	return IsWindow(static_cast<HWND>(window));
}

/*
*	Creates a platform window with the given parameters.
*/
void *const RESTRICT CatalystPlatform::CreatePlatformWindow(const char *const RESTRICT name, const uint32 width, const uint32 height, const WindowType window_type) NOEXCEPT
{
	RegisterWindowInfo();

	HWND window{ nullptr };

	switch (window_type)
	{
		case WindowType::DEFAULT:
		{
			//Define constants.
			constexpr DWORD STYLE{ WS_CAPTION | WS_SYSMENU };

			//Set up the rectangle for the window.
			RECT rectangle;

			rectangle.left = 0;
			rectangle.top = 0;
			rectangle.right = width;
			rectangle.bottom = height;

			//Adjust the rectangle.
			AdjustWindowRect(&rectangle, STYLE, FALSE);

			window = CreateWindow
			(
				CatalystPlatformWindows::_WindowInfo.lpszClassName,
				_T(name),
				STYLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rectangle.right - rectangle.left,
				rectangle.bottom - rectangle.top,
				nullptr,
				nullptr,
				CatalystPlatformWindows::_Instance,
				nullptr
			);

			break;
		}

		case WindowType::FULLSCREEN:
		{
			window = CreateWindow
			(
				CatalystPlatformWindows::_WindowInfo.lpszClassName,
				_T(name),
				WS_POPUP | WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				width,
				height,
				nullptr,
				nullptr,
				CatalystPlatformWindows::_Instance,
				nullptr
			);

			break;
		}

		case WindowType::POPUP:
		{
			//Define constants.
			constexpr DWORD STYLE{ WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU };

			//Set up the rect.
			RECT rect
			{
				0,
				0,
				static_cast<LONG>(width),
				static_cast<LONG>(height)
			};

			//Adjust the rect.
			AdjustWindowRect(&rect, STYLE, FALSE);

			//Create the window.
			window = CreateWindow
			(
				CatalystPlatformWindows::_WindowInfo.lpszClassName,
				_T(name),
				WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rect.right - rect.left,
				rect.bottom - rect.top,
				nullptr,
				nullptr,
				CatalystPlatformWindows::_Instance,
				nullptr
			);

			break;
		}
	}

	if (window)
	{
		//Show the window.
		ShowWindow(window, CatalystPlatformWindows::_ShowCommand);

		//Update the window.
		UpdateWindow(window);
	}

	return window;
}

/*
*	Creates a child platform window with the given parameters.
*/
void *const RESTRICT CatalystPlatform::CreateChildPlatformWindow(void *const RESTRICT parent, const uint32 width, const uint32 height) NOEXCEPT
{
	RegisterWindowInfo();

	return CreateWindowEx
	(
		0,
		CatalystPlatformWindows::_WindowInfo.lpszClassName,
		nullptr,
		WS_CHILD,
		0,
		0,
		width,
		height,
		static_cast<HWND>(parent),
		nullptr,
		CatalystPlatformWindows::_Instance,
		nullptr
	);
}

/*
*	Sets the size of a platform window.
*/
void CatalystPlatform::SetPlatformWindowSize(void *const RESTRICT window, const uint32 width, const uint32 height) NOEXCEPT
{
	SetWindowPos
	(
		static_cast<HWND>(window),
		nullptr,
		0,
		0,
		width,
		height,
		SWP_NOMOVE | SWP_NOZORDER
	);
}

/*
*	Shows the given platform window.
*/
void CatalystPlatform::ShowPlatformWindow(void *const RESTRICT window) NOEXCEPT
{
	ShowWindow(static_cast<HWND>(window), SW_SHOW);
}

/*
*	Hides the given platform window.
*/
void CatalystPlatform::HidePlatformWindow(void *const RESTRICT window) NOEXCEPT
{
	ShowWindow(static_cast<HWND>(window), SW_HIDE);
}

/*
*	Destroys a platform window.
*/
void CatalystPlatform::DestroyPlayformWindow(void *const RESTRICT window) NOEXCEPT
{
	DestroyWindow(static_cast<HWND>(window));
}

/*
*	Returns whether or not the window is in focus.
*/
bool CatalystPlatform::IsWindowInFocus(void *const RESTRICT window) NOEXCEPT
{
	if (window)
	{
		return GetFocus() == static_cast<HWND>(window);
	}

	else
	{
		return CatalystWindowsData::_IsWindowInFocus;
	}
}

/*
*	Returns the default resolution.
*/
void CatalystPlatform::GetDefaultResolution(uint32 *const RESTRICT width, uint32 *const RESTRICT height) NOEXCEPT
{
	//Retrieve the desktop window.
	const HWND desktop_window{ GetDesktopWindow() };

	if (!desktop_window)
	{
		ASSERT(false, "Panik!");

		*width = 1'920;
		*height = 1'080;

		return;
	}

	//Get the window rectangle.
	RECT window_rectangle;
	GetWindowRect(desktop_window, &window_rectangle);

	//Set the width/height.
	*width = window_rectangle.right;
	*height = window_rectangle.bottom;
}

/*
*	Returns the default refresh rate.
*/
float32 CatalystPlatform::GetDefaultRefreshRate() NOEXCEPT
{
	DEVMODE dev_mode;

	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev_mode);

	return static_cast<float32>(dev_mode.dmDisplayFrequency);
}

/*
*	Updates the given button state for the given button.
*/
FORCE_INLINE void UpdateButtonState(const uint16 button, ButtonState *const RESTRICT button_state) NOEXCEPT
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

/*
*	Updates the given mouse state for the given window.
*	If window is nullptr, then the main window will be used.
*/
void CatalystPlatform::UpdateMouseState(MouseState *const RESTRICT mouse_state, void *const RESTRICT window) NOEXCEPT
{
	//Cache the window.
	const HWND _window{ window ? static_cast<HWND>(window) : CatalystPlatformWindows::_MainWindow };

	//Don't update if the window is not in focus, but only if it is the main window. If we have an explicit window passed in, always update.
	if (!window && !CatalystPlatform::IsWindowInFocus(nullptr))
	{
		return;
	}

	//Copy the previous X and Y positions.
	mouse_state->_PreviousX = mouse_state->_CurrentX;
	mouse_state->_PreviousY = mouse_state->_CurrentY;

	//Get the current cursor position.
	POINT point;

	if (GetCursorPos(&point))
	{
		if (ScreenToClient(_window, &point))
		{
			RECT rectangle;

			if (GetClientRect(_window, &rectangle))
			{
				mouse_state->_CurrentX = static_cast<float32>(point.x) / static_cast<float32>(rectangle.right - rectangle.left);
				mouse_state->_CurrentY = 1.0f - static_cast<float32>(point.y) / static_cast<float32>(rectangle.bottom - rectangle.top);

				mouse_state->_DeltaX = mouse_state->_CurrentX - mouse_state->_PreviousX;
				mouse_state->_DeltaY = mouse_state->_CurrentY - mouse_state->_PreviousY;
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

	//Special handling for if the cursor is not shown.
	if (!CatalystPlatform::IsCursorShown())
	{
		//Imagine that the cursor is always at the center of the screen.
		mouse_state->_CurrentX = mouse_state->_CurrentY = mouse_state->_PreviousX = mouse_state->_PreviousY = 0.5f;

		//Get the current cursor position.
		POINT point;

		if (GetCursorPos(&point))
		{
			if (ScreenToClient(_window, &point))
			{
				RECT rectangle;

				if (GetClientRect(_window, &rectangle))
				{
					//Calculate the center and always place the cursor at the center of the screen.
					POINT center;
					POINT adjusted_center;

					center.x = adjusted_center.x = rectangle.left + ((rectangle.right - rectangle.left) / 2);
					center.y = adjusted_center.y = rectangle.top + ((rectangle.bottom - rectangle.top) / 2);

					if (ClientToScreen(_window, &adjusted_center))
					{
						SetCursorPos(adjusted_center.x, adjusted_center.y);

						const float current_x{ BaseMath::Clamp<float>(static_cast<float>(point.x) / static_cast<float>(rectangle.right - rectangle.left), 0.0f, 1.0f) };
						const float current_y{ BaseMath::Clamp<float>(1.0f - static_cast<float>(point.y) / static_cast<float>(rectangle.bottom - rectangle.top), 0.0f, 1.0f) };

						//Don't bother moving the cursor if it hasn't moved N pixels to account for integer rounding errors.
						mouse_state->_DeltaX = (current_x - 0.5f) * static_cast<float>(BaseMath::Absolute<int32>(point.x - center.x) > 0);
						mouse_state->_DeltaY = (current_y - 0.5f) * static_cast<float>(BaseMath::Absolute<int32>(point.y - center.y) > 0);
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
	UpdateButtonState(VK_LBUTTON, &mouse_state->_Left);
	UpdateButtonState(VK_MBUTTON, &mouse_state->_ScrollWheel);
	UpdateButtonState(VK_RBUTTON, &mouse_state->_Right);

	//Update the scroll wheel step.
	mouse_state->_ScrollWheelStep = CatalystPlatform::GetAndResetScrollWheelStep();
}

/*
*	Returns the number of input characters for this frame.
*/
NO_DISCARD uint64 CatalystPlatform::NumberOfInputCharacters() NOEXCEPT
{
	return CatalystPlatformWindows::_InputCharacters.Size();
}

/*
*	Returns the input character at the given index for this frame.
*/
NO_DISCARD char CatalystPlatform::InputCharacterAt(const uint64 index) NOEXCEPT
{
	return CatalystPlatformWindows::_InputCharacters[index];
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

/*
*	Returns whether or not the cursor is shown.
*/
bool CatalystPlatform::IsCursorShown() NOEXCEPT
{
	return CatalystWindowsData::_CursorCurrentVisibility;
}

/*
*	Returns the scroll wheel step and resets it.
*/
int8 CatalystPlatform::GetAndResetScrollWheelStep() NOEXCEPT
{
	const int8 old{ CatalystWindowsData::_ScrollWheelStep };

	CatalystWindowsData::_ScrollWheelStep = 0;

	return old;
}

/*
*	Retrieves the user name.
*/
bool CatalystPlatform::RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT
{
	char username[UNLEN + 1];
	DWORD username_length{ UNLEN + 1 };

	if (GetUserNameA(username, &username_length))
	{
		output->SetLength(username_length);

		for (uint64 i{ 0 }; i < username_length; ++i)
		{
			(*output)[i] = username[i];
		}

		return true;
	}

	else
	{
		return false;
	}
}

void CatalystPlatform::ShowBannerAd() NOEXCEPT
{
	//Do nothing. (:
}

/*
*	Shows an interstitial ad.
*/
void CatalystPlatform::ShowInterstitialAd() NOEXCEPT
{
	//Do nothing. (:
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT format, ...) NOEXCEPT
{
	/*
	*	To guard against invalid format specifiers, we put the message in a staging buffer first and filter it.
	*/
	char format_buffer[4096];

	{
		const uint64 format_length{ strlen(format) };

		for (uint64 i{ 0 }; i < format_length; ++i)
		{
			//Guard against '%_'.
			if (format[i] == '%' && (i + 1) < format_length && format[i + 1] == '_')
			{
				format_buffer[i] = '_';
			}

			//Guard against '%%'.
			if (format[i] == '%' && (i + 1) < format_length && format[i + 1] == '%')
			{
				format_buffer[i] = '_';
			}

			else
			{
				format_buffer[i] = format[i];
			}
		}

		format_buffer[format_length] = '\0';
	}

	va_list variadic_arguments{ nullptr };
	va_start(variadic_arguments, format);

	char buffer[4096];

	const int32 characters_needed{ vsnprintf(buffer, ARRAY_LENGTH(buffer), format, variadic_arguments) };

	if (characters_needed > ARRAY_LENGTH(buffer))
	{
		int x = 0;
	}

	va_end(variadic_arguments);

	const uint64 length{ strlen(buffer) };

	buffer[length] = '\n';
	buffer[length + 1] = '\0';

	OutputDebugString(buffer);
}
#endif

/*
*	The crash handler.
*/
LONG WINAPI CatalystPlatformWindows::CrashHandler(EXCEPTION_POINTERS *exception_info) NOEXCEPT
{
	LOG_FATAL("Crash detected!");

	LOG_INFORMATION("Callstack:");

	char *callstack{ b_stacktrace_get_string() };
	LOG_INFORMATION(callstack);
	free(callstack);

	LogSystem::Instance->Flush();

	return EXCEPTION_EXECUTE_HANDLER;
}
#endif