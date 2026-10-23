#if defined(AUDIO_PLUGIN)
//Header file.
#include <Audio/Plugins/Editor/PluginWindow.h>

//Platform specific stuff.
#if defined(CATALYST_PLATFORM_WINDOWS)
//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Constants.
#define TIMER_ID (0)
#define TIMER_FREQUENCY (1)

//The window info state.
bool PLUGIN_WINDOW_INFO_REGISTERED{ false };
WNDCLASSEX PLUGIN_WINDOW_INFO;

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
			Audio::PluginWindow *const RESTRICT plugin_window{ static_cast<Audio::PluginWindow *const RESTRICT>(create_state->lpCreateParams) };

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
				Audio::PluginWindow *const RESTRICT plugin_window{ reinterpret_cast<Audio::PluginWindow *const RESTRICT>(GetWindowLongPtr(window, GWLP_USERDATA)) };

				if (plugin_window)
				{
					//Call the update function.
					plugin_window->CallUpdateFunction();
				}
			}

			break;
		}

		case WM_MOUSEWHEEL:
		{
			//Retrieve the plugin window.
			Audio::PluginWindow *const RESTRICT plugin_window{ reinterpret_cast<Audio::PluginWindow *const RESTRICT>(GetWindowLongPtr(window, GWLP_USERDATA)) };

			if (plugin_window)
			{
				plugin_window->SetScrollWheelStep(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
			}

			break;
		}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

/*
*	Registers the window info.
*	Returns if the registration succeeded.
*/
FORCE_INLINE NO_DISCARD bool RegisterPluginWindowInfo() NOEXCEPT
{
	//If the window is already explicitly registered, return true.
	if (PLUGIN_WINDOW_INFO_REGISTERED)
	{
		return true;
	}

	//Set up the window info.
	PLUGIN_WINDOW_INFO.cbSize = sizeof(WNDCLASSEX);
	PLUGIN_WINDOW_INFO.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	PLUGIN_WINDOW_INFO.lpfnWndProc = PluginWindowProcedure;
	PLUGIN_WINDOW_INFO.cbClsExtra = 0;
	PLUGIN_WINDOW_INFO.cbWndExtra = 0;
	PLUGIN_WINDOW_INFO.hInstance = CatalystPlatformWindows::_Instance;
	PLUGIN_WINDOW_INFO.hIcon = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);
	PLUGIN_WINDOW_INFO.hCursor = LoadCursor(NULL, IDC_ARROW);
	PLUGIN_WINDOW_INFO.hbrBackground = 0;
	PLUGIN_WINDOW_INFO.lpszMenuName = NULL;
	PLUGIN_WINDOW_INFO.lpszClassName = _T("Catalyst Plugin Window");
	PLUGIN_WINDOW_INFO.hIconSm = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);

	//Due to .dll loading/unloading shenanigans, the class may already be registered - Check that.
	{
		WNDCLASSEX existing_class{ };
		existing_class.cbSize = sizeof(WNDCLASSEX);

		if (GetClassInfoEx(CatalystPlatformWindows::_Instance, _T("Catalyst Plugin Window"), &existing_class))
		{
			//The class existis, but we need to check that it's actually the same class.
			const bool identical_window_procedure{ existing_class.lpfnWndProc == PLUGIN_WINDOW_INFO.lpfnWndProc };

			//If they are identical, the class is already registered.
			if (identical_window_procedure)
			{
				PLUGIN_WINDOW_INFO_REGISTERED = true;

				return true;
			}
		}
	}

	//Try to register the class.
	if (RegisterClassEx(&PLUGIN_WINDOW_INFO))
	{
		PLUGIN_WINDOW_INFO_REGISTERED = true;
	}

	//Return if the window info was registered.
	return PLUGIN_WINDOW_INFO_REGISTERED;
}

/*
*	Tries to create an OpenGL plugin window.
*/
FORCE_INLINE NO_DISCARD void *const RESTRICT TryCreateOpenGLPluginWindow
(
	Audio::PluginWindow *const RESTRICT plugin_window,
	void *const RESTRICT parent_window,
	const Resolution resolution
) NOEXCEPT
{
	//Register the window info.
	if (!RegisterPluginWindowInfo())
	{
		return nullptr;
	}

	//Create the temporary window.
	HWND temporary_window
	{
		CreateWindow
		(
			PLUGIN_WINDOW_INFO.lpszClassName,
			"Plugin Window",
			0,
			0,
			0,
			1,
			1,
			nullptr,
			nullptr,
			CatalystPlatformWindows::_Instance,
			plugin_window
		)
	};

	if (!temporary_window)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Retrieve the temporary device context.
	HDC temporary_device_context{ GetDC(temporary_window) };

	if (!temporary_device_context)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set up the pixel format descriptor.
	PIXELFORMATDESCRIPTOR temporary_pixel_format_descriptor;
	Memory::Set(&temporary_pixel_format_descriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));

	temporary_pixel_format_descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	temporary_pixel_format_descriptor.nVersion = 1;
	temporary_pixel_format_descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	temporary_pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
	temporary_pixel_format_descriptor.cColorBits = 32;
	temporary_pixel_format_descriptor.cAlphaBits = 8;
	temporary_pixel_format_descriptor.cDepthBits = 24;

	//Choose the temporary pixel format.
	const int32 temporary_pixel_format{ ChoosePixelFormat(temporary_device_context, &temporary_pixel_format_descriptor) };

	if (temporary_pixel_format == 0)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set the pixel format.
	if (!SetPixelFormat(temporary_device_context, temporary_pixel_format, &temporary_pixel_format_descriptor))
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Create the temporary render context.
	HGLRC temporary_render_context{ wglCreateContext(temporary_device_context) };

	if (!temporary_render_context)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Make the temporary render context current.
	if (!wglMakeCurrent(temporary_device_context, temporary_render_context))
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Retrieve the necessary function pointers.
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{ reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB")) };

	if (!wglChoosePixelFormatARB)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{ reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB")) };

	if (!wglCreateContextAttribsARB)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Now create the final window.
	HWND final_window
	{
		CreateWindow
		(
			PLUGIN_WINDOW_INFO.lpszClassName,
			"Plugin Window",
			WS_CHILD,
			0,
			0,
			resolution._Width,
			resolution._Height,
			static_cast<HWND>(parent_window),
			nullptr,
			CatalystPlatformWindows::_Instance,
			plugin_window
		)
	};

	if (!final_window)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Retrieve the final device context.
	HDC final_device_context{ GetDC(final_window) };

	if (!final_device_context)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set up the final pixel attributes.
	constexpr int32 FINAL_PIXEL_ATTRIBUTES[]
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	//Choose the pixel format.
	int32 final_pixel_format;
	uint32 final_number_of_formats;

	const BOOL could_choose_final_pixel_format{ wglChoosePixelFormatARB(final_device_context, FINAL_PIXEL_ATTRIBUTES, nullptr, 1, &final_pixel_format, &final_number_of_formats) };

	if (could_choose_final_pixel_format == 0 || final_number_of_formats == 0)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set up the final pixel format descriptor.
	PIXELFORMATDESCRIPTOR final_pixel_format_descriptor;
	const int32 describe_final_pixel_format_result{ DescribePixelFormat(final_device_context, final_pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &final_pixel_format_descriptor) };

	if (describe_final_pixel_format_result == 0)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set the final pixel format.
	if (!SetPixelFormat(final_device_context, final_pixel_format, &final_pixel_format_descriptor))
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set up the final render context.
	constexpr int32 CONTEXT_ATTRIBUTES[]
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	HGLRC final_render_context{ wglCreateContextAttribsARB(final_device_context, 0, CONTEXT_ATTRIBUTES) };

	if (!final_render_context)
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Clean up the temporary stuff.
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(temporary_render_context);
	ReleaseDC(temporary_window, temporary_device_context);
	DestroyWindow(temporary_window);

	//Make the final render context current.
	if (!wglMakeCurrent(final_device_context, final_render_context))
	{
		const DWORD error{ GetLastError() };
		BREAKPOINT();
		return nullptr;
	}

	//Set the device context on the window.
	plugin_window->SetDeviceContext(final_device_context);

	//Retrieve the function pointers for OpenGL.
	OpenGL::RetrieveOpenGLFunctionPointers();

	//Return the final window. (:
	return final_window;
}

/*
*	Creates the plugin window.
*/
FORCE_INLINE NO_DISCARD void *const RESTRICT CreatePluginWindow
(
	Audio::PluginWindow *const RESTRICT plugin_window,
	void *const RESTRICT parent_window,
	const Resolution resolution
) NOEXCEPT
{
	//Register the window info.
	if (!RegisterPluginWindowInfo())
	{
		return nullptr;
	}

	//Try to create an OpenGL window.
	void *RESTRICT window{ TryCreateOpenGLPluginWindow(plugin_window, parent_window, resolution) };

	//If we couldn't create an OpenGL window, create a regular one.
	if (!window)
	{
		window = CreateWindow
		(
			PLUGIN_WINDOW_INFO.lpszClassName,
			"Plugin Window",
			WS_CHILD,
			0,
			0,
			resolution._Width,
			resolution._Height,
			static_cast<HWND>(parent_window),
			nullptr,
			CatalystPlatformWindows::_Instance,
			plugin_window
		);
	}

	else
	{
		//Let the window know that it supports OpenGL.
		plugin_window->SetSupportsOpenGL(true);
	}

	return window;
}

/*
*	Swaps the buffers for the window with the given device context.
*/
FORCE_INLINE void SwapBuffersForWindow(void *const RESTRICT device_context) NOEXCEPT
{
	SwapBuffers(static_cast<HDC>(device_context));
}

/*
*	Renders into the given window.
*/
FORCE_INLINE void RenderToWindow(HWND window, const Resolution resolution, const uint32 *const RESTRICT data) NOEXCEPT
{
	//Begin the paint.
	HDC device_context = GetDC(window);

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
	ReleaseDC(window, device_context);
}
#endif

namespace Audio
{

	/*
	*	Sets the update function.
	*/
	void PluginWindow::SetUpdateFunction(const UpdateFunction function, void *const RESTRICT arguments) NOEXCEPT
	{
		//Set the update function.
		_UpdateFunction = function;

		//Set the update arguments.
		_UpdateArguments = arguments;
	}

	/*
	*	Initializes this plugin window.
	*/
	void PluginWindow::Initialize(const Resolution resolution, void *const RESTRICT parent_window) NOEXCEPT
	{
		//Set the resolution.
		_Resolution = resolution;

		//Create the plugin window.
		_Window = CreatePluginWindow(this, parent_window, resolution);
	}

	/*
	*	Shows this plugin window.
	*/
	void PluginWindow::Show()
	{
		CatalystPlatform::ShowPlatformWindow(_Window);
		SetTimer(static_cast<HWND>(_Window), TIMER_ID, TIMER_FREQUENCY, nullptr);
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
		if (_Window)
		{
			CatalystPlatform::DestroyPlayformWindow(_Window);

			_Window = nullptr;
		}
	}

	/*
	*	Calls the update function.
	*/
	void PluginWindow::CallUpdateFunction() NOEXCEPT
	{
		_UpdateFunction(_UpdateArguments);
	}

	/*
	*	Returns the mouse state.
	*/
	NO_DISCARD const MouseState *const RESTRICT PluginWindow::GetMouseState() NOEXCEPT
	{
		//Update the mouse state.
		CatalystPlatform::UpdateMouseState(&_MouseState, _Window);

		//Set the scroll wheel step.
		_MouseState._ScrollWheelStep = GetAndResetScrollWheelStep();

		//Return the mouse state.
		return &_MouseState;
	}

	/*
	*	Swaps the buffers.
	*/
	void PluginWindow::SwapBuffers() NOEXCEPT
	{
		SwapBuffersForWindow(_DeviceContext);
	}

	/*
	*	Renders to this window.
	*/
	void PluginWindow::RenderTo(const uint32 *const RESTRICT data) NOEXCEPT
	{
		RenderToWindow(static_cast<HWND>(_Window), _Resolution, data);
	}

}
#endif