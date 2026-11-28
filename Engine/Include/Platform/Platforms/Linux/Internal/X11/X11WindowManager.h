#pragma once

//Core.
#include "Platform/Platforms/Linux/Internal/X11/X11WindowData.h"
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

		//Determine the parent.
		Window parent{ _Context._RootWindow };

		if (parameters._Parent != Platform::INVALID_WINDOW_HANDLE)
		{
			parent = HandleToWindow(parameters._Parent);
		}

		//Create the window.
		Window window{ None };
		Any<X11WindowData::X11_OPEN_GL_CONTEXT_SIZE> open_gl_context;
		bool supports_open_gl{ false };

		if (parameters._SupportOpenGL)
		{
			window = TryCreateOpenGLWindow(parameters, parent, &open_gl_context);

			if (window != None)
			{
				supports_open_gl = true;
			}
		}

		if (window == None)
		{
			window = XCreateWindow
			(
				_Context._Display,
				parent,
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
			);
		}

		if (window == None)
		{
			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Set the title.
		XStoreName(_Context._Display, window, parameters._Title.Data());

		//Show the window.
		XMapWindow(_Context._Display, window);

		//Flush the display.
		XFlush(_Context._Display);

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the parameters.
		window_data->_Parameters = parameters;

		//Set the window.
		window_data->_X11WindowData._Window = window;

		//set the OpenGL context.
		window_data->_X11WindowData._OpenGLContext = open_gl_context;

		//Set the final resolution.
		window_data->_FinalResolution = parameters._Resolution;

		//Set whether or now this window supports OpenGL.
		window_data->_SupportsOpenGL = supports_open_gl;

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
		Memory::Copy(&result, &static_cast<WindowData *const RESTRICT>(window)->_X11WindowData._Window, sizeof(Window));
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
	void PrepareOpenGL(const Platform::WindowHandle window) NOEXCEPT;

	/*
	*	Finalizes OpenGL for the given window.
	*/
	void FinalizeOpenGL(const Platform::WindowHandle window) NOEXCEPT;

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
		Display *_Display{ nullptr };

		//The default screen.
		int32 _DefaulScreen{ INT32_MAXIMUM };

		//The root window.
		Window _RootWindow{ None };

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
		_Context._Display = XOpenDisplay(nullptr);

		if (!_Context._Display)
		{
			return false;
		}

		//Retrieve the default screen.
		_Context._DefaulScreen = XDefaultScreen(_Context._Display);

		//Retrieve the root window.
		_Context._RootWindow = XRootWindow(_Context._Display, _Context._DefaulScreen);

		//The context is now initialized.
		_Context._Initialized = true;

		//Return success!
		return true;
	}

	/*
	*	Converts an X11 window to a handle.
	*/
	FORCE_INLINE NO_DISCARD Platform::PlatformNativeWindowHandle WindowToHandle(const Window window) NOEXCEPT
	{
		Platform::PlatformNativeWindowHandle result;
		Memory::Copy(&result, &window, sizeof(Window));
		return result;
	}

	/*
	*	Converts a handle to an X11 window.
	*/
	FORCE_INLINE NO_DISCARD Window HandleToWindow(const Platform::PlatformNativeWindowHandle window) NOEXCEPT
	{
		Window result;
		Memory::Copy(&result, &window, sizeof(Window));
		return result;
	}

	/*
	*	Tries to create an OpenGL window.
	*/
	NO_DISCARD Window TryCreateOpenGLWindow
	(
		const Platform::WindowManager::CreateParameters &parameters,
		const Window parent,
		Any<X11WindowData::X11_OPEN_GL_CONTEXT_SIZE> *const RESTRICT open_gl_context
	) NOEXCEPT;

};