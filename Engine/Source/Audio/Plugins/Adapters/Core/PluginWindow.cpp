//Header file.
#include <Audio/Plugins/Adapters/Core/PluginWindow.h>

//Audio.
#include <Audio/Plugins/Adapters/Core/PluginRenderer.h>

/*
*	Initializes this plugin window.
*/
void PluginWindow::Initialize(PluginRenderer *const RESTRICT renderer) NOEXCEPT
{
	//Set the renderer.
	_Renderer = renderer;
}

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
		case WM_NCCREATE:
		{
			//Retrieve the create state.
			const CREATESTRUCT *const RESTRICT create_state{ reinterpret_cast<const CREATESTRUCT *const RESTRICT>(lParam) };

			//Retrieve the plugin window.
			PluginWindow *const RESTRICT plugin_window{ static_cast<PluginWindow *const RESTRICT>(create_state->lpCreateParams) };

			//Associate this plugin window with the window.
			SetWindowLongPtr
			(
				window,
				GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(plugin_window)
			);

			break;
		}

		case WM_TIMER:
		{
			if (wParam == TIMER_ID)
			{
				//Retrieve the plugin window.
				PluginWindow *const RESTRICT plugin_window{ reinterpret_cast<PluginWindow *const RESTRICT>(GetWindowLongPtr(window, GWLP_USERDATA)) };

				if (plugin_window)
				{
					//Update the renderer.
					plugin_window->GetRenderer()->Update();

					//Invalidate the window to receive a PAINT call.
					InvalidateRect(window, nullptr, FALSE);
				}
			}

			break;
		}

		case WM_PAINT:
		{
			//Retrieve the plugin window.
			PluginWindow *const RESTRICT plugin_window{ reinterpret_cast<PluginWindow *const RESTRICT>(GetWindowLongPtr(window, GWLP_USERDATA)) };

			if (plugin_window)
			{
				//Retrieve the renderer.
				const PluginRenderer *const RESTRICT renderer{ plugin_window->GetRenderer() };

				//If we are doing software renderering, blit the pixels into the window.
				if (renderer->IsDoingSoftwareRendering())
				{
					//Retrieve the resolution.
					const Resolution resolution{ plugin_window->GetResolution() };

					//Retrieve the data.
					const uint32 *const RESTRICT data{ renderer->GetSoftwareRenderData() };

					//Begin the paint.
					PAINTSTRUCT paint_state;
					HDC device_context{ BeginPaint(window, &paint_state) };

					//Set up the bitmap info.
					BITMAPINFO bitmap_info{ };

					bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					bitmap_info.bmiHeader.biWidth = resolution._Width;
					bitmap_info.bmiHeader.biHeight = resolution._Height;
					bitmap_info.bmiHeader.biPlanes = 1;
					bitmap_info.bmiHeader.biBitCount = 32;
					bitmap_info.bmiHeader.biCompression = BI_RGB;

					//Retrieve the client rectangle.
					RECT client_rectangle{ };
					GetClientRect(window, &client_rectangle);

					//Do the paint!
					StretchDIBits
					(
						device_context,
						0,
						0,
						client_rectangle.right,
						client_rectangle.bottom,
						0,
						0,
						resolution._Width,
						resolution._Height,
						data,
						&bitmap_info,
						DIB_RGB_COLORS,
						SRCCOPY
					);

					//End the paint.
					EndPaint(window, &paint_state);
				}
			}
			
			return 0;
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
	//If the window is already explicitly registered, return true.
	if (WINDOW_INFO_REGISTERED)
	{
		return true;
	}

	//Set up the window info.
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
	WINDOW_INFO.lpszClassName = _T("Catalyst Plugin Window");
	WINDOW_INFO.hIconSm = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);

	//Due to .dll loading/unloading shenanigans, the class may already be registered - Check that.
	{
		WNDCLASSEX existing_class{ };
		existing_class.cbSize = sizeof(WNDCLASSEX);

		if (GetClassInfoEx(CatalystPlatformWindows::_Instance, _T("Catalyst Plugin Window"), &existing_class))
		{
			//The class existis, but we need to check that it's actually the same class.
			const bool identical_window_procedure{ existing_class.lpfnWndProc == WINDOW_INFO.lpfnWndProc };

			//If they are identical, the class is already registered.
			if (identical_window_procedure)
			{
				WINDOW_INFO_REGISTERED = true;

				return true;
			}
		}
	}

	//Try to register the class.
	if (RegisterClassEx(&WINDOW_INFO))
	{
		WINDOW_INFO_REGISTERED = true;
	}

	//Return if the window info was registered.
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

	//Set the resolution.
	_Resolution = resolution;

	//Create the window.
	_Window = CreateWindowEx
	(
		0,
		WINDOW_INFO.lpszClassName,
		nullptr,
		WS_CHILD,
		0,
		0,
		_Resolution._Width,
		_Resolution._Height,
		static_cast<HWND>(parent_window),
		nullptr,
		CatalystPlatformWindows::_Instance,
		this
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