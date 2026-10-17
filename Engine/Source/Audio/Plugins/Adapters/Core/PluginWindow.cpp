//Header file.
#include <Audio/Plugins/Adapters/Core/PluginWindow.h>

#if defined(CATALYST_PLATFORM_WINDOWS)
//Math.
#include <Math/Core/CatalystRandomMath.h>

//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

//Constants.
#define TIMER_ID (0)

//The window info state.
bool WINDOW_INFO_REGISTERED{ false };
WNDCLASSEX WINDOW_INFO;

/*
*	Window procedure handling.
*/
LRESULT CALLBACK PluginWindowProcedure
(
	_In_ HWND   window,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) NOEXCEPT
{
	switch (message)
	{
		case WM_TIMER:
		{
			if (wParam == TIMER_ID)
			{
				//TODO:Run the UI update loop.

				//Invalidate the window to receive a PAINT call.
				InvalidateRect(window, nullptr, FALSE);
			}

			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT paint_state;
			HDC device_context{ BeginPaint(window, &paint_state) };

			RECT client_rectangle;
			GetClientRect(window, &client_rectangle);

			HBRUSH brush{ CreateSolidBrush(RGB(CatalystRandomMath::RandomIntegerInRange<int32>(0, 255), CatalystRandomMath::RandomIntegerInRange<int32>(0, 255), CatalystRandomMath::RandomIntegerInRange<int32>(0, 255)))};

			if (brush)
			{
				FillRect(device_context, &client_rectangle, brush);
				DeleteObject(brush);
			}

			EndPaint(window, &paint_state);
			
			break;
		}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

/*
*	Registers the window info.
*	Returns if the registration succeeded.
*/
FORCE_INLINE NO_DISCARD bool RegisterWindowInfo() NOEXCEPT
{
	if (WINDOW_INFO_REGISTERED)
	{
		return true;
	}

	WINDOW_INFO.cbSize = sizeof(WNDCLASSEX);
	WINDOW_INFO.style = 0;
	WINDOW_INFO.lpfnWndProc = PluginWindowProcedure;
	WINDOW_INFO.cbClsExtra = 0;
	WINDOW_INFO.cbWndExtra = 0;
	WINDOW_INFO.hInstance = CatalystPlatformWindows::_Instance;
	WINDOW_INFO.hIcon = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);
	WINDOW_INFO.hCursor = LoadCursor(NULL, IDC_ARROW);
	WINDOW_INFO.hbrBackground = 0;
	WINDOW_INFO.lpszMenuName = NULL;
	WINDOW_INFO.lpszClassName = _T("Plugin Window");
	WINDOW_INFO.hIconSm = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);

	if (RegisterClassEx(&WINDOW_INFO))
	{
		WINDOW_INFO_REGISTERED = true;
	}

	return WINDOW_INFO_REGISTERED;
}

/*
*	Creates this plugin window.
*	Returns if the creation succeeded.
*/
NO_DISCARD bool PluginWindow::Create(void *const RESTRICT parent_window, const Resolution resolution) NOEXCEPT
{
	//Register the window info.
	if (!RegisterWindowInfo())
	{
		return false;
	}

	//Create the window.
	_Window = CreateWindowEx
	(
		0,
		WINDOW_INFO.lpszClassName,
		nullptr,
		WS_CHILD,
		0,
		0,
		resolution._Width,
		resolution._Height,
		static_cast<HWND>(parent_window),
		nullptr,
		CatalystPlatformWindows::_Instance,
		nullptr
	);

	//Return if the window could be created.
	return _Window != nullptr;
}
#endif

/*
*	Shows this plugin window.
*/
void PluginWindow::Show()
{
	CatalystPlatform::ShowPlatformWindow(_Window);
	SetTimer(static_cast<HWND>(_Window), TIMER_ID, 16, nullptr);
}

/*
*	Hides this plugin window.
*/
void PluginWindow::Hide()
{
	KillTimer(static_cast<HWND>(_Window), TIMER_ID);
	CatalystPlatform::HidePlatformWindow(_Window);
}

/*
*	Destroys this plugin window.
*/
void PluginWindow::Destroy()
{
	CatalystPlatform::DestroyPlayformWindow(_Window);
}