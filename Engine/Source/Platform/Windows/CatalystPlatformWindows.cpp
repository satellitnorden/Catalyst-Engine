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
HWND CatalystPlatformWindows::_Window;
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
	*	Updates during the post update phase.
	*/
	void PostUpdate() NOEXCEPT
	{
		//Determine if the window is in focus.
		CatalystWindowsData::_IsWindowInFocus = GetFocus() == CatalystPlatformWindows::_Window;

		//Clear the keyboard events.
		CatalystPlatformWindows::_KeyDownEvents.Clear();
		CatalystPlatformWindows::_KeyUpEvents.Clear();
		CatalystPlatformWindows::_InputCharacters.Clear();

		//Process messages.
		MSG message;

		while (PeekMessage(&message, CatalystPlatformWindows::_Window, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

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

					if (GetClientRect(CatalystPlatformWindows::_Window, &rectangle))
					{
						POINT minimum{ rectangle.left, rectangle.bottom };
						POINT maximum{ rectangle.right, rectangle.top };

						if (ClientToScreen(CatalystPlatformWindows::_Window, &minimum) && ClientToScreen(CatalystPlatformWindows::_Window, &maximum))
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
	//Create the window info.
	WNDCLASSEX windowInfo;

	windowInfo.cbSize = sizeof(WNDCLASSEX);
	windowInfo.style = 0;
	windowInfo.lpfnWndProc = WindowProcedure;
	windowInfo.cbClsExtra = 0;
	windowInfo.cbWndExtra = 0;
	windowInfo.hInstance = CatalystPlatformWindows::_Instance;
	windowInfo.hIcon = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);
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

	if (CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._InitialFullScreen)
	{
		Resolution resolution;

		if (CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Valid())
		{
			resolution = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Get();
		}

		else
		{
			CatalystPlatform::GetDefaultResolution(&resolution._Width, &resolution._Height);
		}

		CatalystPlatformWindows::_Window = CreateWindow
		(
			windowInfo.lpszClassName,
			_T(CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data()),
			WS_POPUP | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			resolution._Width,
			resolution._Height,
			nullptr,
			nullptr,
			CatalystPlatformWindows::_Instance,
			nullptr
		);
	}

	else
	{
		char window_name_buffer[256];

#if defined(CATALYST_EDITOR)
		sprintf_s(window_name_buffer, "Catalyst Editor - %s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#else
		sprintf_s(window_name_buffer, "%s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#endif

		Resolution resolution;

		if (CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Valid())
		{
			resolution = CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution.Get();
		}

		else
		{
			CatalystPlatform::GetDefaultResolution(&resolution._Width, &resolution._Height);
		}

		CatalystPlatformWindows::_Window = CreateWindow
		(
			windowInfo.lpszClassName,
			_T(window_name_buffer),
			WS_MAXIMIZE | WS_SYSMENU | WS_EX_TRANSPARENT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			resolution._Width,
			resolution._Height,
			nullptr,
			nullptr,
			CatalystPlatformWindows::_Instance,
			nullptr
		);
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	if (!CatalystPlatformWindows::_Window)
	{
		const DWORD result{ GetLastError() };

		BREAKPOINT();
	}
#endif

	//Show the window.
	ShowWindow(CatalystPlatformWindows::_Window, CatalystPlatformWindows::_ShowCommand);

	//Update the window.
	UpdateWindow(CatalystPlatformWindows::_Window);

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
	CatalystWindowsLogic::PostUpdate();
}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{

}

/*
*	Returns whether or not the window is in focus.
*/
bool CatalystPlatform::IsWindowInFocus() NOEXCEPT
{
	return CatalystWindowsData::_IsWindowInFocus;
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
	va_start(variadic_arguments, format_buffer);

	char buffer[4096];

	vsprintf_s(buffer, format_buffer, variadic_arguments);

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