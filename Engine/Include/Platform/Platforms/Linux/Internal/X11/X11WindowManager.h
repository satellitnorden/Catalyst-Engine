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
        //Open the display, if we haven't already.
        if (!_Display)
        {
            _Display = X11::XOpenDisplay(nullptr);
        }

        //Can't proceed without the display.
        if (!_Display)
        {
            return Platform::INVALID_WINDOW_HANDLE;
        }

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the parameters.
		window_data->_Parameters = parameters;

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
		//Linux does not understand the concept of a "window" handle.
		return Platform::INVALID_WINDOW_HANDLE;
	}

	/*
	*	Returns a platform native display handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
	*/
	NO_DISCARD Platform::PlatformNativeWindowHandle GetDisplayHandle(const Platform::WindowHandle window) NOEXCEPT
	{
		///Linux does not understand the concept of a "display" handle.
		return Platform::INVALID_WINDOW_HANDLE;
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

    //The window data lock.
    SpinLock &_WindowDataLock;

    //The window data.
    DynamicArray<WindowData *RESTRICT> &_WindowData;

    //The display (or X server).
    X11::Display *_Display{ nullptr };

};