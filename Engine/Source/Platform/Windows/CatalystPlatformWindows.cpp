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
#include <Math/Core/CatalystBaseMath.h>
#include <Math/General/Vector.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Windows.
#include <Xinput.h>

//Static variable definitions.
HINSTANCE CatalystPlatform::_Instance;
int32 CatalystPlatform::_ShowCommand;
HWND CatalystPlatform::_Window;

//Catalyst Windows data.
namespace CatalystWindowsData
{
	//Denotes whether or not the window is in focus.
	bool _IsWindowInFocus{ true };

	//Denotes the cursor's requested visibility.
	bool _CursorRequestedVisibility{ true };

	//Denotes the cursor's current visibility.
	bool _CursorCurrentVisibility{ true };

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
		CatalystWindowsData::_IsWindowInFocus = GetFocus() == CatalystPlatform::_Window;

		//Process messages.
		MSG message;

		while (PeekMessage(&message, CatalystPlatform::_Window, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//Update the cursor.
		if (CatalystWindowsData::_CursorCurrentVisibility != CatalystWindowsData::_CursorRequestedVisibility)
		{
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
	//Set the requested visibility.
	CatalystWindowsData::_CursorRequestedVisibility = visibility;

	//If the cursor is hidden/shown from the main thread, if can be hidden/shown immediately.
	if (Concurrency::CurrentThread::IsMainThread())
	{
		if (CatalystWindowsData::_CursorCurrentVisibility != CatalystWindowsData::_CursorRequestedVisibility)
		{
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
*	Pre-initializes the platform.
*/
void CatalystPlatform::PreInitialize() NOEXCEPT
{

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

#if !defined(CATALYST_EDITOR)
	if (CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._InitialFullScreen)
	{
		_Window = CreateWindow(	windowInfo.lpszClassName,
								_T(CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data()),
								WS_POPUP | WS_VISIBLE,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Width,
								CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Height,
								nullptr,
								nullptr,
								_Instance,
								nullptr);
	}

	else
#endif
	{
		char window_name_buffer[256];

#if defined(CATALYST_EDITOR)
		sprintf_s(window_name_buffer, "Catalyst Editor - %s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#else
		sprintf_s(window_name_buffer, "%s", CatalystEngineSystem::Instance->GetProjectConfiguration()->_GeneralConfiguration._ProjectName.Data());
#endif

		_Window = CreateWindow(	windowInfo.lpszClassName,
								_T(window_name_buffer),
								WS_MAXIMIZE | WS_SYSMENU | WS_EX_TRANSPARENT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Width,
								CatalystEngineSystem::Instance->GetProjectConfiguration()->_RenderingConfiguration._Resolution._Height,
								nullptr,
								nullptr,
								_Instance,
								nullptr);
	}

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

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT)
	{
		CatalystWindowsLogic::PostUpdate();
	},
	nullptr,
	UpdatePhase::POST,
	UpdatePhase::PRE,
	true,
	false);
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
Resolution CatalystPlatform::GetDefaultResolution() NOEXCEPT
{
	//Retrieve the desktop window.
	const HWND desktop_window{ GetDesktopWindow() };

	if (!desktop_window)
	{
		ASSERT(false, "Panik!");

		return Resolution(1'920, 1'080);
	}

	//Get the window rectangle.
	RECT window_rectangle;
	GetWindowRect(desktop_window, &window_rectangle);

	//Return the resolution.
	return Resolution(window_rectangle.right, window_rectangle.bottom);
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