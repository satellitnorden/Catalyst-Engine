#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/Core/WindowManager.h>

//Concurrency.
#include <Concurrency/SpinLock.h>

//Platform.
#include <Platform/Platforms/Linux/Internal/Wayland/WaylandWindowManager.h>
#include <Platform/Platforms/Linux/Internal/X11/X11WindowManager.h>
#include <Platform/Platforms/Linux/Internal/Windows/WindowData.h>

//The window data lock.
SpinLock _WindowDataLock;

//The window data.
DynamicArray<WindowData *RESTRICT> _WindowData;

//The master window data.
WindowData *RESTRICT _MasterWindowData{ nullptr };

//The Wayland window manager.
WaylandWindowManager _WaylandWindowManager{ _WindowDataLock, _WindowData };

//The number of Wayland windows.
uint32 _NumberOfWaylandWindows{ 0 };

//The X11 window manager.
X11WindowManager _X11WindowManager{ _WindowDataLock, _WindowData };

//The number of X11 windows.
uint32 _NumberOfX11Windows{ 0 };

namespace Platform
{

	/*
	*	Initializes the window manager.
	*/
	void WindowManager::Initialize() NOEXCEPT
	{
		//Initialize the window managers.
		_WaylandWindowManager.Initialize();
		_X11WindowManager.Initialize();
	}

	/*
	*	Updates the window manager.
	*	Can optionally specify a specific window handle to update, otherwise will update all managed windows.
	*/
	void WindowManager::Update(const WindowHandle window) NOEXCEPT
	{
		//Update the window managers.
		if (_NumberOfWaylandWindows > 0)
		{
			_WaylandWindowManager.Update(window);
		}

		if (_NumberOfX11Windows > 0)
		{
			_X11WindowManager.Update(window);
		}
	}

	/*
	*	Terminates the window manager.
	*/
	void WindowManager::Terminate() NOEXCEPT
	{
		//Terminate the window managers.
		_WaylandWindowManager.Terminate();
		_X11WindowManager.Terminate();

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
		//Call different backends.
		switch (parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				const WindowHandle result{ _WaylandWindowManager.Create(parameters) };

				if (result != INVALID_WINDOW_HANDLE)
				{
					++_NumberOfWaylandWindows;
				}
				
				return result;
			}

			case Backend::X11:
			{
				const WindowHandle result{ _X11WindowManager.Create(parameters) };

				if (result != INVALID_WINDOW_HANDLE)
				{
					++_NumberOfX11Windows;
				}
				
				return result;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return INVALID_WINDOW_HANDLE;
			}
		}
	}

	/*
	*	Returns a platform native window handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "window" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetWindowHandle(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				return _WaylandWindowManager.GetWindowHandle(window);
			}

			case Backend::X11:
			{
				return _X11WindowManager.GetWindowHandle(window);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return INVALID_WINDOW_HANDLE;
			}
		}
	}

	/*
	*	Returns a platform native display handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetDisplayHandle(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				return _WaylandWindowManager.GetDisplayHandle(window);
			}

			case Backend::X11:
			{
				return _X11WindowManager.GetDisplayHandle(window);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return INVALID_WINDOW_HANDLE;
			}
		}
	}

	/*
	*	Returns a platform native surface handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetSurfaceHandle(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				return _WaylandWindowManager.GetSurfaceHandle(window);
			}

			case Backend::X11:
			{
				return _X11WindowManager.GetSurfaceHandle(window);
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				return INVALID_WINDOW_HANDLE;
			}
		}
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

		//Return the final resolution.
		return window_data->_FinalResolution;
	}

	/*
	*	Sets the resolution of the given window.
	*/
	void WindowManager::SetResolution(const WindowHandle window, const Resolution &resolution) NOEXCEPT
	{

	}

	/*
	*	Returns if the given window is alive.
	*/
	NO_DISCARD bool WindowManager::IsAlive(const WindowHandle window) NOEXCEPT
	{
		return true;
	}

	/*
	*	Returns if the given window is focused.
	*/
	NO_DISCARD bool WindowManager::IsFocused(const WindowHandle window) NOEXCEPT
	{
		return true;
	}

	/*
	*	Shows the given window.
	*/
	void WindowManager::Show(const WindowHandle window) NOEXCEPT
	{
		
	}

	/*
	*	Hides the given window.
	*/
	void WindowManager::Hide(const WindowHandle window) NOEXCEPT
	{
		
	}

	/*
	*	Renders to the given window with the given data.
	*/
	void WindowManager::Render(const WindowHandle window, const uint32 *const RESTRICT data) NOEXCEPT
	{
		
	}

	/*
	*	Returns if the given window supports OpenGL.
	*/
	NO_DISCARD bool WindowManager::SupportsOpenGL(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Return if this window supports OpenGL.
		return window_data->_SupportsOpenGL;
	}

	/*
	*	Prepares OpenGL for the given window.
	*/
	void WindowManager::PrepareOpenGL(const WindowHandle window) NOEXCEPT
	{
		ASSERT(SupportsOpenGL(window), "This window does not support OpenGL!");

		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				_WaylandWindowManager.PrepareOpenGL(window);
				
				break;
			}

			case Backend::X11:
			{
				_X11WindowManager.PrepareOpenGL(window);
				
				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Finalizes OpenGL for the given window.
	*/
	void WindowManager::FinalizeOpenGL(const WindowHandle window) NOEXCEPT
	{
		ASSERT(SupportsOpenGL(window), "This window does not support OpenGL!");

		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				_WaylandWindowManager.FinalizeOpenGL(window);
				
				break;
			}

			case Backend::X11:
			{
				_X11WindowManager.FinalizeOpenGL(window);
				
				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
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

		//Call different backends.
		switch (window_data->_Parameters._Backend)
		{
			case Backend::WAYLAND:
			{
				_WaylandWindowManager.Destroy(window);
				
				break;
			}

			case Backend::X11:
			{
				_X11WindowManager.Destroy(window);
				
				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

	/*
	*	Adds window closed callback to the given window.
	*/
	void WindowManager::AddWindowClosedCallback(const WindowHandle window, const WindowClosedCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

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
		//SetTimer(window_data->_Handle, identifier, frequency, nullptr);

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
		//KillTimer(window_data->_Handle, identifier);

		//Remove the callback.
		window_data->_Callbacks.RemoveTimerCallback(identifier);
	}

}
#endif