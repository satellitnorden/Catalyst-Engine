#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Platform/Core/WindowManager.h>

//Core.
#include <Core/General/Pair.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>

//Platform.
#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Third party.
#if (CATALYST_HAS_OPEN_GL)
	#include <gl/wglext.h>
#endif

/*
*	Window data class definition.
*/
class WindowData final
{

public:

	//The parameters.
	Platform::WindowManager::CreateParameters _Parameters;

	//The handle.
	HWND _Handle;

	//The device context.
	HDC _DeviceContext;

	//The render context.
	HGLRC _RenderContext;

	//Denotes whether or not this window supports OpenGL.
	bool _SupportsOpenGL{ false };

	//Denotes whether or not this window is focused.
	bool _Focused{ false };

	//The callbacks.
	Platform::WindowManager::Callbacks _Callbacks;

	//The last mouse position.
	Vector2<float32> _LastMousePosition{ 0.5f, 0.5f };

	//The last mouse button states.
	StaticArray<ButtonState, UNDERLYING(MouseButton::NUMBER_OF_MOUSE_BUTTONS)> _LastMouseButtonStates;

	//The timer identifier generator.
	uint64 _TimerIdentifierGenerator{ 0 };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE WindowData() NOEXCEPT
	{
		for (ButtonState &last_mouse_button_state : _LastMouseButtonStates)
		{
			last_mouse_button_state = ButtonState::RELEASED;
		}
	}

};

//The window data lock.
SpinLock _WindowDataLock;

//The window data.
DynamicArray<WindowData *RESTRICT> _WindowData;

//The master window data.
WindowData *RESTRICT _MasterWindowData{ nullptr };

//Denotes whether or not the window info is registered.
bool _WindowInfoRegistered;

//The window info.
WNDCLASSEX _WindowInfo;

/*
*	Finds the window data for the given handle.
*/
FORCE_INLINE NO_DISCARD WindowData *const RESTRICT FindWindowData(const HWND handle) NOEXCEPT
{
	for (WindowData *const RESTRICT window_data : _WindowData)
	{
		if (window_data->_Handle == handle)
		{
			return window_data;
		}
	}

	return nullptr;
}

/*
*	Finds the keyboard button for the given Windows key code.
*/
FORCE_INLINE NO_DISCARD KeyboardButton FindKeyboardButton(const uint64 key_code) NOEXCEPT
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

		//These two control buttons could be differentiated with 'GetKeyState()', but probably no need right now.
		Pair<uint64, KeyboardButton>(VK_CONTROL, KeyboardButton::LeftControl),
		Pair<uint64, KeyboardButton>(VK_CONTROL, KeyboardButton::RightControl),
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

	for (const Pair<uint64, KeyboardButton> &mapping : MAPPINGS)
	{
		if (mapping._First == key_code)
		{
			return mapping._Second;
		}
	}

	ASSERT(false, "Couldn't find keyboard button!");

	return KeyboardButton::NumberOfKeyboardButtons;
}

/*
*	Returns the mouse position for the given window.
*/
FORCE_INLINE NO_DISCARD Vector2<float32> GetMousePosition(const HWND window) NOEXCEPT
{
	//Get the position.
	Vector2<float32> position{ 0.5f, 0.5f };

	//Get the current cursor position.
	POINT point;

	if (GetCursorPos(&point))
	{
		if (ScreenToClient(window, &point))
		{
			RECT rectangle;

			if (GetClientRect(window, &rectangle))
			{
				position._X = static_cast<float32>(point.x) / static_cast<float32>(rectangle.right - rectangle.left);
				position._Y = 1.0f - static_cast<float32>(point.y) / static_cast<float32>(rectangle.bottom - rectangle.top);
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

	//Return the position.
	return position;
}

/*
*	Updates the mouse state for the given window data.
*/
FORCE_INLINE void UpdateMouseStateForWindowData(WindowData *const RESTRICT window_data) NOEXCEPT
{
	//Retrieve the mouse position for this window.
	const Vector2<float32> mouse_position{ GetMousePosition(window_data->_Handle) };

	//If it has changed, update it and call the callbacks.
	if (window_data->_LastMousePosition != mouse_position)
	{
		window_data->_Callbacks.CallMouseMoveCallbacks(mouse_position);
		window_data->_LastMousePosition = mouse_position;
	}

	//Update the mouse button states.
	{
		constexpr DWORD KEY_CODES[]
		{
			VK_LBUTTON,
			VK_MBUTTON,
			VK_RBUTTON
		};

		for (uint64 i{ 0 }; i < window_data->_LastMouseButtonStates.Size(); ++i)
		{
			const bool pressed{ static_cast<bool>(GetKeyState(KEY_CODES[i]) & 0x8000) };

			if (pressed && window_data->_LastMouseButtonStates[i] == ButtonState::RELEASED)
			{
				window_data->_Callbacks.CallMouseButtonDownCallbacks(static_cast<MouseButton>(i));
				window_data->_LastMouseButtonStates[i] = ButtonState::PRESSED;
			}

			else if (!pressed && window_data->_LastMouseButtonStates[i] == ButtonState::PRESSED)
			{
				window_data->_Callbacks.CallMouseButtonUpCallbacks(static_cast<MouseButton>(i));
				window_data->_LastMouseButtonStates[i] = ButtonState::RELEASED;
			}
		}
	}
}

/*
*	The window procedure function.
*/
LRESULT CALLBACK WindowProcedure
(
	_In_ HWND   window,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) NOEXCEPT
{
	switch (message)
	{
		case WM_DESTROY:
		{
			//Find the window data.
			WindowData *const RESTRICT window_data{ FindWindowData(window) };

			if (window_data)
			{
				//Call the callbacks.
				window_data->_Callbacks.CallWindowClosedCallbacks();
			}

			return 0;
		}

		case WM_KILLFOCUS:
		{
			for (WindowData *const RESTRICT window_data : _WindowData)
			{
				if (window_data->_Handle == window)
				{
					window_data->_Focused = false;
				}
			}

			return 0;
		}

		case WM_SETFOCUS:
		{
			for (WindowData *const RESTRICT window_data : _WindowData)
			{
				if (window_data->_Handle == window)
				{
					window_data->_Focused = true;
				}

				else
				{
					window_data->_Focused = false;
				}
			}

			return 0;
		}

		case WM_KEYDOWN:
		{
			//Find the window data.
			WindowData *const RESTRICT window_data{ FindWindowData(window) };

			//Find the keyboard button.
			const KeyboardButton keyboard_button{ FindKeyboardButton(wParam) };

			if (keyboard_button != KeyboardButton::NumberOfKeyboardButtons)
			{
				window_data->_Callbacks.CallKeyboardButtonDownCallbacks(keyboard_button);
			}

			return 0;
		}

		case WM_KEYUP:
		{
			//Find the window data.
			WindowData *const RESTRICT window_data{ FindWindowData(window) };

			//Find the keyboard button.
			const KeyboardButton keyboard_button{ FindKeyboardButton(wParam) };

			if (keyboard_button != KeyboardButton::NumberOfKeyboardButtons)
			{
				window_data->_Callbacks.CallKeyboardButtonUpCallbacks(keyboard_button);
			}

			return 0;
		}

		case WM_TIMER:
		{
			//Find the window data.
			if (WindowData *const RESTRICT window_data{ FindWindowData(window) })
			{
				/*
				*	A bit of a hack, but currently only audio plugins use this feature,
				*	so get the mouse position here and pass it along,
				*	since audio plugins don't run the regular update loop.
				*/
			#if defined(AUDIO_PLUGIN)
				UpdateMouseStateForWindowData(window_data);
			#endif

				//Call the timer callback.
				window_data->_Callbacks.CallTimerCallback(wParam);
			}

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			//Find the window data.
			WindowData *const RESTRICT window_data{ FindWindowData(window) };

			//Calculate the step.
			const int8 step{ static_cast<int8>(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA) };

			//Call the callbacks.
			window_data->_Callbacks.CallMouseScrollWheelCallbacks(step);

			return 0;
		}

		/*
		case WM_CHAR:
		{
			CatalystPlatformWindows::_InputCharacters.Emplace((char)(wParam & 255));

			break;
		}
		*/
	}

	return DefWindowProc(window, message, wParam, lParam);
}

/*
*	Registers the window info.
*	Returns if the registration succeded.
*/
FORCE_INLINE NO_DISCARD bool RegisterWindowInfo() NOEXCEPT
{
	//Early out if we have already registered.
	if (_WindowInfoRegistered)
	{
		return true;
	}

	//Fill in the window info.
	_WindowInfo.cbSize = sizeof(WNDCLASSEX);
	_WindowInfo.style = 0;
	_WindowInfo.lpfnWndProc = WindowProcedure;
	_WindowInfo.cbClsExtra = 0;
	_WindowInfo.cbWndExtra = 0;
	_WindowInfo.hInstance = CatalystPlatformWindows::_Instance;
	_WindowInfo.hIcon = LoadIcon(CatalystPlatformWindows::_Instance, IDI_APPLICATION);
	_WindowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WindowInfo.hbrBackground = 0;
	_WindowInfo.lpszMenuName = NULL;
	_WindowInfo.lpszClassName = _T("Catalyst Engine Window");
	_WindowInfo.hIconSm = LoadIcon(_WindowInfo.hInstance, IDI_APPLICATION);

	//Due to .dll loading/unloading shenanigans, the class may already be registered - Check that.
	{
		WNDCLASSEX existing_class{ };
		existing_class.cbSize = sizeof(WNDCLASSEX);

		if (GetClassInfoEx(CatalystPlatformWindows::_Instance, _T("Catalyst Engine Window"), &existing_class))
		{
			//The class existis, but we need to check that it's actually the same class.
			const bool identical_window_procedure{ existing_class.lpfnWndProc == _WindowInfo.lpfnWndProc };

			//If they are identical, the class is already registered.
			if (identical_window_procedure)
			{
				_WindowInfoRegistered = true;

				return true;
			}
		}
	}

	//Try to register the class.
	if (RegisterClassEx(&_WindowInfo))
	{
		_WindowInfoRegistered = true;
	}

	//Return if the window info was registered.
	return _WindowInfoRegistered;
}

#if (CATALYST_HAS_OPEN_GL)
/*
*	Tries to create an OpenGL plugin window.
*/
FORCE_INLINE NO_DISCARD HWND TryCreateOpenGLPluginWindow
(
	const Platform::WindowManager::CreateParameters &parameters,
	const DWORD style,
	const RECT rectangle,
	HDC *const RESTRICT device_context,
	HGLRC *const RESTRICT render_context
) NOEXCEPT
{
	//Register the window info.
	if (!RegisterWindowInfo())
	{
		return nullptr;
	}

	//Create the temporary window.
	HWND temporary_window
	{
		CreateWindow
		(
			_WindowInfo.lpszClassName,
			"Temporary Window",
			0,
			0,
			0,
			1,
			1,
			nullptr,
			nullptr,
			CatalystPlatformWindows::_Instance,
			nullptr
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
			_WindowInfo.lpszClassName,
			_T(parameters._Title.Data()),
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rectangle.right - rectangle.left,
			rectangle.bottom - rectangle.top,
			parameters._Parent != Platform::INVALID_WINDOW_HANDLE ? static_cast<HWND>(parameters._Parent) : nullptr,
			nullptr,
			CatalystPlatformWindows::_Instance,
			nullptr
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

	//Set the device context.
	*device_context = final_device_context;

	//Set the render context.
	*render_context = final_render_context;

	//Retrieve the function pointers for OpenGL.
	OpenGL::RetrieveOpenGLFunctionPointers();

	//Return the final window. (:
	return final_window;
}
#endif

namespace Platform
{

	/*
	*	Initializes the window manager.
	*/
	void WindowManager::Initialize() NOEXCEPT
	{
		//Nothing to do here. (:
	}

	/*
	*	Updates the window manager.
	*	Can optionally specify a specific window handle to update, otherwise will update all managed windows.
	*/
	void WindowManager::Update(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ window != INVALID_WINDOW_HANDLE ? static_cast<WindowData *const RESTRICT>(window) : nullptr };

		//Dispatch messages.
		MSG message;

		while (PeekMessage(&message, window_data ? window_data->_Handle : NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		//We want mouse move events to be able to be sent even outside the window, so can't rely on the window procedure, need to query that explicitly.
		if (window_data)
		{
			UpdateMouseStateForWindowData(window_data);
		}
		
		else
		{
			for (WindowData *const RESTRICT _window_data : _WindowData)
			{
				//Only update for focused windows.
				if (!_window_data->_Focused)
				{
					continue;
				}

				//If this window has no mouse move callbacks added, no need to query the mouse position.
				if (_window_data->_Callbacks._NumberOfMouseMoveCallbacks == 0)
				{
					continue;
				}

				UpdateMouseStateForWindowData(_window_data);
			}
		}
	}

	/*
	*	Terminates the window manager.
	*/
	void WindowManager::Terminate() NOEXCEPT
	{
		//Free all window data.
		for (WindowData *const RESTRICT window_data : _WindowData)
		{
			delete window_data;
		}

		_WindowData.Clear();
	}

	/*
	*	Creates a window with the given parameters.
	*/
	NO_DISCARD WindowHandle WindowManager::Create(const CreateParameters &parameters) NOEXCEPT
	{
		//Try to register the window info.
		if (!RegisterWindowInfo())
		{
			return INVALID_WINDOW_HANDLE;
		}

		//Set up the style.
		DWORD style{ 0 };

		if (parameters._Parent != INVALID_WINDOW_HANDLE)
		{
			style = WS_CHILD;
		}
		
		else
		{
			if (parameters._FullScreen)
			{
				style = WS_POPUP;
			}

			else
			{
				style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
			}
		}

		//Set up the rectangle for the window.
		RECT rectangle;

		rectangle.left = 0;
		rectangle.top = 0;
		rectangle.right = parameters._Resolution._Width;
		rectangle.bottom = parameters._Resolution._Height;

		if (parameters._FullScreen || !parameters._ResolutionIncludesPlatformElements)
		{
			AdjustWindowRect(&rectangle, style, FALSE);
		}
		
		//Create the window!
		HWND window{ nullptr };
		HDC device_context{ nullptr };
		HGLRC render_context{ nullptr };

#if defined(CATALYST_HAS_OPEN_GL)
		if (parameters._SupportOpenGL)
		{
			window = TryCreateOpenGLPluginWindow(parameters, style, rectangle, &device_context, &render_context);
		}
#endif

		if (!window)
		{
			window = CreateWindow
			(
				_WindowInfo.lpszClassName,
				_T(parameters._Title.Data()),
				style,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				rectangle.right - rectangle.left,
				rectangle.bottom - rectangle.top,
				parameters._Parent != Platform::INVALID_WINDOW_HANDLE ? static_cast<HWND>(parameters._Parent) : nullptr,
				nullptr,
				CatalystPlatformWindows::_Instance,
				nullptr
			);
		}

		//If we couldn't create the window, return an invalid handle.
		if (!window)
		{
			return INVALID_WINDOW_HANDLE;
		}

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the parameters.
		window_data->_Parameters = parameters;

		//Set the handle.
		window_data->_Handle = window;

		//Set the device context.
		window_data->_DeviceContext = device_context;

		//Set the render context.
		window_data->_RenderContext = render_context;

		//Set whether or not this window supports OpenGL.
		window_data->_SupportsOpenGL = render_context != nullptr;

		//Add the window data.
		{
			SCOPED_LOCK(_WindowDataLock);
			_WindowData.Emplace(window_data);
		}

		//Show the window.
		ShowWindow(window, SW_SHOW);

		//Update the window.
		UpdateWindow(window);

		//Return the window data!
		return window_data;
	}

	/*
	*	Returns the backend of the given window.
	*/
	NO_DISCARD WindowManager::Backend WindowManager::GetBackend(const WindowHandle window) NOEXCEPT
	{
		return Backend::WIN32;
	}

	/*
	*	Returns a platform native window handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "window" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetWindowHandle(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Return the window.
		return window_data->_Handle;
	}

	/*
	*	Returns a platform native display handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetDisplayHandle(const WindowHandle window) NOEXCEPT
	{
		//Windows does not understand the concept of a "display" handle.
		return INVALID_WINDOW_HANDLE;
	}

	/*
	*	Returns a platform native surface handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetSurfaceHandle(const WindowHandle window) NOEXCEPT
	{
		//Windows does not understand the concept of a "surface" handle.
		return INVALID_WINDOW_HANDLE;
	}

	/*
	*	Returns the resolution of the given window.
	*	The resolution passed in to 'Create()' may not be the final resolution of the window,
	*	due to some platforms making other decisions,
	*	so call this to query the actual resolution of the renderable area of the window.
	*/
	NO_DISCARD Resolution WindowManager::GetResolution(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		/*
		*	TODO: Query if the actual resolution is different, but most of the time on Windows,
		*	it will be the same as requested when creating the window.
		*/
		return window_data->_Parameters._Resolution;
	}

	/*
	*	Sets the resolution of the given window.
	*/
	void WindowManager::SetResolution(const WindowHandle window, const Resolution &resolution) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Set the resolution.
		SetWindowPos
		(
			window_data->_Handle,
			nullptr,
			0,
			0,
			resolution._Width,
			resolution._Height,
			SWP_NOMOVE | SWP_NOZORDER
		);
	}

	/*
	*	Returns if the given window is alive.
	*/
	NO_DISCARD bool WindowManager::IsAlive(const WindowHandle window) NOEXCEPT
	{
		return IsWindow(static_cast<WindowData *const RESTRICT>(window)->_Handle);
	}

	/*
	*	Returns if the given window is focused.
	*/
	NO_DISCARD bool WindowManager::IsFocused(const WindowHandle window) NOEXCEPT
	{
		return static_cast<WindowData *const RESTRICT>(window)->_Focused;
	}

	/*
	*	Shows the given window.
	*/
	void WindowManager::Show(const WindowHandle window) NOEXCEPT
	{
		ShowWindow(static_cast<WindowData *const RESTRICT>(window)->_Handle, SW_SHOW);
	}

	/*
	*	Hides the given window.
	*/
	void WindowManager::Hide(const WindowHandle window) NOEXCEPT
	{
		ShowWindow(static_cast<WindowData *const RESTRICT>(window)->_Handle, SW_HIDE);
	}

	/*
	*	Renders to the given window with the given data.
	*/
	void WindowManager::Render(const WindowHandle window, const uint32 *const RESTRICT data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Begin the paint.
		HDC device_context = GetDC(window_data->_Handle);

		//Set up the bitmap info.
		BITMAPINFO bitmap_info{ };

		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info.bmiHeader.biWidth = window_data->_Parameters._Resolution._Width;
		bitmap_info.bmiHeader.biHeight = window_data->_Parameters._Resolution._Height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		//Retrieve the client rectangle.
		RECT client_rectangle{ };
		GetClientRect(window_data->_Handle, &client_rectangle);

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
			window_data->_Parameters._Resolution._Width,
			window_data->_Parameters._Resolution._Height,
			data,
			&bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		);

		//End the paint.
		ReleaseDC(window_data->_Handle, device_context);
	}

	/*
	*	Returns if the given window supports OpenGL.
	*/
	NO_DISCARD bool WindowManager::SupportsOpenGL(const WindowHandle window) NOEXCEPT
	{
		return static_cast<WindowData *const RESTRICT>(window)->_SupportsOpenGL;
	}

	/*
	*	Prepares OpenGL for the given window.
	*/
	void WindowManager::PrepareOpenGL(const WindowHandle window) NOEXCEPT
	{
		ASSERT(SupportsOpenGL(window), "This window does not support OpenGL!");

	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Set the current OpenGL context.
		wglMakeCurrent(window_data->_DeviceContext, window_data->_RenderContext);
	#endif
	}

	/*
	*	Finalizes OpenGL for the given window.
	*/
	void WindowManager::FinalizeOpenGL(const WindowHandle window) NOEXCEPT
	{
		ASSERT(SupportsOpenGL(window), "This window does not support OpenGL!");

	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Swap the buffers.
		SwapBuffers(window_data->_DeviceContext);
	#endif
	}

	/*
	*	Returns the master window.
	*/
	NO_DISCARD const WindowHandle WindowManager::GetMasterWindow() NOEXCEPT
	{
		return _MasterWindowData;
	}

	/*
	*	Sets the master window.
	*/
	void WindowManager::SetMasterWindow(const WindowHandle window) NOEXCEPT
	{
		//Set the master window data.
		_MasterWindowData = static_cast<WindowData *const RESTRICT>(window);
	}

	/*
	*	Destroys the given window.
	*/
	void WindowManager::Destroy(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Destroy the window.
		DestroyWindow(window_data->_Handle);

		//Remove it from the window data.
		_WindowData.Erase<false>(window_data);

		//Finally destroy the window data.
		delete window_data;
	}

	/*
	*	Adds window closed callback to the given window.
	*/
	void WindowManager::AddWindowClosedCallback(const WindowHandle window, const WindowClosedCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData* const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddWindowClosedCallback(callback, user_data);
	}

	/*
	*	Adds a keyboard button down callback to the given window.
	*/
	void WindowManager::AddKeyboardButtonDownCallback(const WindowHandle window, const KeyboardButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddKeyboardButtonDownCallback(callback, user_data);
	}

	/*
	*	Adds a keyboard button up callback to the given window.
	*/
	void WindowManager::AddKeyboardButtonUpCallback(const WindowHandle window, const KeyboardButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddKeyboardButtonUpCallback(callback, user_data);
	}

	/*
	*	Adds a mouse move callback to the given window.
	*/
	void WindowManager::AddMouseMoveCallback(const WindowHandle window, const MouseMoveCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddMouseMoveCallback(callback, user_data);
	}

	/*
	*	Adds a mouse button down callback to the given window.
	*/
	void WindowManager::AddMouseButtonDownCallback(const WindowHandle window, const MouseButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddMouseButtonDownCallback(callback, user_data);
	}

	/*
	*	Adds a mouse button up callback to the given window.
	*/
	void WindowManager::AddMouseButtonUpCallback(const WindowHandle window, const MouseButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddMouseButtonUpCallback(callback, user_data);
	}


	/*
	*	Adds a mouse scroll wheel callback to the given window.
	*/
	void WindowManager::AddMouseScrollWheelCallback(const WindowHandle window, const MouseScrollWheelCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddMouseScrollWheelCallback(callback, user_data);
	}

	/*
	*	Adds a timer callback to the given window.
	*	Frequency is denoted in milliseconds.
	*	Returns an identifier that can be used to remove the timer later.
	*/
	NO_DISCARD uint64 WindowManager::AddTimerCallback(const WindowHandle window, const uint64 frequency, const TimerCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Generate the identifier.
		const uint64 identifier{ window_data->_TimerIdentifierGenerator++ };

		//Add the callback.
		window_data->_Callbacks.AddTimerCallback(identifier, callback, user_data);

		//Set the timer.
		SetTimer(window_data->_Handle, identifier, frequency, nullptr);

		//Return the identifier.
		return identifier;
	}

	/*
	*	Removes the timer with the given identifier from the given window.
	*/
	void WindowManager::RemoveTimerCallback(const WindowHandle window, const uint64 identifier) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Kill the timer.
		KillTimer(window_data->_Handle, identifier);

		//Remove the callback.
		window_data->_Callbacks.RemoveTimerCallback(identifier);
	}

}
#endif