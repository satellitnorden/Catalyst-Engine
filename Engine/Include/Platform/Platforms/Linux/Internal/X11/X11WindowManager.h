#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>

//Platform.
#include <Platform/Core/WindowManager.h>
#include <Platform/Platforms/Linux/Internal/X11/X11Core.h>
#include <Platform/Platforms/Linux/Internal/Windows/WindowData.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>
#include <X11/Xlib.h>

class X11WindowManager final
{

public:

    /*
    *   Default constructor.
    */
    FORCE_INLINE X11WindowManager(SpinLock &window_data_lock, DynamicArray<WindowData *RESTRICT> &window_data) NOEXCEPT
        :
        _WindowDataLock(window_data_lock),
        _WindowData(window_data)
    {
        
    }

    /*
    *   Initializes the Wayland window manager.
    */
    FORCE_INLINE void Initialize() NOEXCEPT
    {
        //Nothing to do here. (:
    }

    /*
    *   Updates the Wayland window manager.
    */
    FORCE_INLINE void Update(const Platform::WindowHandle window) NOEXCEPT
    {
        
    }

    /*
    *   Terminates the Wayland window manager.
    */
    FORCE_INLINE void Terminate() NOEXCEPT
    {
        //Nothing to do here. (:
    }

    /*
	*	Creates a window with the given parameters.
	*/
	NO_DISCARD Platform::WindowHandle Create(const Platform::WindowManager::CreateParameters &parameters) NOEXCEPT
	{
        //Try to initialize the context.
		if (!InitializeContext())
		{
			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Create the window.
		const X11::Window window
		{
			X11::XCreateWindow
			(
				_Context._Display,
				_Context._RootWindow,
				0,
				0,
				parameters._Resolution._Width,
				parameters._Resolution._Height,
				0,
				CopyFromParent,
				InputOutput,
				CopyFromParent,
				0,
				nullptr
			)
		};

		if (window == None)
		{
			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Set the title.
		X11::XStoreName(_Context._Display, window, parameters._Title.Data());

		//Show the window.
		X11::XMapWindow(_Context._Display, window);

		//Flush the display.
		X11::XFlush(_Context._Display);

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the parameters.
		window_data->_Parameters = parameters;

		//Set the window.
		window_data->_X11WindowData._Window = window;

		//Set the final resolution.
		window_data->_FinalResolution = parameters._Resolution;

		//Add the window data.
		{
			SCOPED_LOCK(_WindowDataLock);
			_WindowData.Emplace(window_data);
		}

		//Return the window data!
		return window_data;
	}

	/*
	*	Returns a platform native window handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "window" may differ between platforms.
	*/
	NO_DISCARD Platform::PlatformNativeWindowHandle GetWindowHandle(const Platform::WindowHandle window) NOEXCEPT
	{
		Platform::PlatformNativeWindowHandle result;
		Memory::Copy(&result, &static_cast<WindowData *const RESTRICT>(window)->_X11WindowData._Window, sizeof(X11::Window));
		return result;
	}

	/*
	*	Returns a platform native display handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
	*/
	NO_DISCARD Platform::PlatformNativeWindowHandle GetDisplayHandle(const Platform::WindowHandle window) NOEXCEPT
	{
		return _Context._Display;
	}

	/*
	*	Returns a platform native surface handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
	*/
	NO_DISCARD Platform::PlatformNativeWindowHandle GetSurfaceHandle(const Platform::WindowHandle window) NOEXCEPT
	{
		//Linux does not understand the concept of a "surface" handle.
		return Platform::INVALID_WINDOW_HANDLE;
	}

	/*
	*	Prepares OpenGL for the given window.
	*/
	void PrepareOpenGL(const Platform::WindowHandle window) NOEXCEPT
	{
	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };
	#endif
	}

	/*
	*	Finalizes OpenGL for the given window.
	*/
	void FinalizeOpenGL(const Platform::WindowHandle window) NOEXCEPT
	{
	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };
	#endif
	}

    /*
	*	Destroys the given window.
	*/
	void Destroy(const Platform::WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Remove it from the window data.
		_WindowData.Erase<false>(window_data);

		//Finally destroy the window data.
		delete window_data;
	}

private:

	/*
	*	Context class definition.
	*/
	class Context final
	{

	public:

		//Denotes whether or not this context is initialized.
		bool _Initialized{ false };

		//The display.
		X11::Display *_Display{ nullptr };

		//The default screen.
		int32 _DefaulScreen{ INT32_MAXIMUM };

		//The root window.
		X11::Window _RootWindow{ None };

	};

    //The window data lock.
    SpinLock &_WindowDataLock;

    //The window data.
    DynamicArray<WindowData *RESTRICT> &_WindowData;

    //The context.
	Context _Context;

	/*
	*	Initializes the context.
	*/
	FORCE_INLINE NO_DISCARD bool InitializeContext() NOEXCEPT
	{
		//Return if the context is already initialized.
		if (_Context._Initialized)
		{
			return true;
		}

		//Try to open the display.
		_Context._Display = X11::XOpenDisplay(nullptr);

		if (!_Context._Display)
		{
			return false;
		}

		//Retrieve the default screen.
		_Context._DefaulScreen = X11::XDefaultScreen(_Context._Display);

		//Retrieve the root window.
		_Context._RootWindow = X11::XRootWindow(_Context._Display, _Context._DefaulScreen);

		//The context is now initialized.
		_Context._Initialized = true;

		//Return success!
		return true;
	}

};