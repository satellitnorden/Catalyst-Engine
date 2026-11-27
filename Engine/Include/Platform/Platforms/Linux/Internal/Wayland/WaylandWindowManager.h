#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>

//Platform.
#include <Platform/Core/WindowManager.h>
#include <Platform/Platforms/Linux/Internal/Wayland/WaylandContext.h>
#include <Platform/Platforms/Linux/Internal/Windows/WindowData.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <linux/input-event-codes.h>
#include <errno.h>
#include <poll.h>

/*
*	The XDG surface listener configure function.
*/
void XDGSurfaceListenerConfigure
(
	void *data,
	xdg_surface *xdg_surface,
	uint32_t serial
)
{
	PRINT_TO_OUTPUT("XDGSurfaceListen called!");

	//Cache the window data.
	WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(data) };

	//Acknowledge the configure.
	xdg_surface_ack_configure(xdg_surface, serial);

	//This window is now configured!
	window_data->_WaylandWindowData._Configured = true;
}

//The XDG surface listener.
static constexpr xdg_surface_listener XDG_SURFACE_LISTENER
{
	XDGSurfaceListenerConfigure
};

/*
*	The XDG top level listener configure function.
*/
void XDGTopLevelListenerConfigure
(
	void *data,
	xdg_toplevel *xdg_toplevel,
	int32_t width,
	int32_t height,
	wl_array *states
)
{
	PRINT_TO_OUTPUT("XDGTopLevelListenerConfigure called!");

	//Cache the window data.
	WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(data) };

	//Set the width of the final resolution.
	if (width > 0)
	{
		window_data->_FinalResolution._Width = width;
	}

	else
	{
		window_data->_FinalResolution._Width = window_data->_Parameters._Resolution._Width;
	}

	//Set the height of the final resolution.
	if (height > 0)
	{
		window_data->_FinalResolution._Height = height;
	}

	else
	{
		window_data->_FinalResolution._Height = window_data->_Parameters._Resolution._Height;
	}
}

/*
*	The XDG top level listener close function.
*/
void XDGTopLevelListenerClose
(
	void *data,
	xdg_toplevel *xdg_toplevel
)
{
	PRINT_TO_OUTPUT("XDGTopLevelListenerClose called!");

	//Cache the window data.
	WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(data) };

	if (window_data)
	{
		//Call the callbacks.
		window_data->_Callbacks.CallWindowClosedCallbacks();
	}
}

/*
*	The XDG top level listener configure bounds function.
*/
void XDGTopLevelListenerConfigureBounds
(
	void *data,
	xdg_toplevel *xdg_toplevel,
	int32_t width,
	int32_t height
)
{
	PRINT_TO_OUTPUT("XDGTopLevelListenerConfigureBounds called!");
}

/*
*	The XDG top level listener WM capabilities function.
*/
void XDGTopLevelListenerWMCapabilities
(
	void *data,
	xdg_toplevel *xdg_toplevel,
	wl_array *capabilities
)
{
	PRINT_TO_OUTPUT("XDGTopLevelListenerWMCapabilities called!");
}

//The XDG top level listener.
static constexpr xdg_toplevel_listener XDG_TOP_LEVEL_LISTENER
{
	XDGTopLevelListenerConfigure,
	XDGTopLevelListenerClose,
	XDGTopLevelListenerConfigureBounds,
	XDGTopLevelListenerWMCapabilities
};

class WaylandWindowManager final
{

public:

    /*
    *   Default constructor.
    */
    FORCE_INLINE WaylandWindowManager(SpinLock &window_data_lock, DynamicArray<WindowData *RESTRICT> &window_data) NOEXCEPT
        :
        _WindowDataLock(window_data_lock),
        _WindowData(window_data),
        _WaylandContext(window_data)
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
        //Dispatch the display.
		if (_WaylandContext._Display)
		{
			//Try to prepare a read.
			while (wl_display_prepare_read(_WaylandContext._Display) != 0)
			{
				if (wl_display_dispatch_pending(_WaylandContext._Display) < 0)
				{
					//There was an error, return.
					return;
				}
			}

			//Flush the display.
			const int32 flush_result{ wl_display_flush(_WaylandContext._Display) };

			//Determine if we need to write.
			bool write_needed{ false };
			
			if (flush_result < 0)
			{
				if (errno == EAGAIN)
				{
					write_needed = true;
				}

				else
				{
					wl_display_cancel_read(_WaylandContext._Display);

					return;
				}
			}

			//Poll the Wayland display socket.
			pollfd fd{ };
			fd.fd = wl_display_get_fd(_WaylandContext._Display);
			fd.events = POLLIN;

			if (write_needed)
			{
				fd.events |= POLLOUT;
			}
			
			const int32 poll_result{ poll(&fd, 1, 0) };

			if (poll_result < 0)
			{
				wl_display_cancel_read(_WaylandContext._Display);

				return;
			}

			//Read the incoming events.
			if (poll_result > 0 && (fd.revents & POLLIN))
			{
				if (wl_display_read_events(_WaylandContext._Display) < 0)
				{
					return;
				}
			}
			else
			{
				wl_display_cancel_read(_WaylandContext._Display);
			}

			//Dispatch events.
			if (wl_display_dispatch_pending(_WaylandContext._Display) < 0)
			{
				return;
			}
		}
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
		//Initialize the Wayland context.
		if (!_WaylandContext.Initialize())
		{
			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Create the surface.
		wl_surface *const RESTRICT surface{ wl_compositor_create_surface(_WaylandContext._Compositor) };

		if (!surface)
		{
			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Create the XDG surface.
		xdg_surface *const RESTRICT xdg_surface{ xdg_wm_base_get_xdg_surface(_WaylandContext._XDGWindowManager, surface) };

		if (!xdg_surface)
		{
			wl_surface_destroy(surface);

			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Create the XDG top level.
		xdg_toplevel *const RESTRICT xdg_top_level{ xdg_surface_get_toplevel(xdg_surface) };

		if (!xdg_top_level)
		{
			xdg_surface_destroy(xdg_surface);
			wl_surface_destroy(surface);

			return Platform::INVALID_WINDOW_HANDLE;
		}

		//Create the decoration.
		zxdg_toplevel_decoration_v1 *const RESTRICT decoration{ parameters._FullScreen ? nullptr : zxdg_decoration_manager_v1_get_toplevel_decoration(_WaylandContext._ZXDGDecorationManager, xdg_top_level) };

		//Add the decoration.
		if (!parameters._FullScreen)
		{
			zxdg_toplevel_decoration_v1_set_mode(decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
		}

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the parameters.
		window_data->_Parameters = parameters;

		//Set the surface.
		window_data->_WaylandWindowData._Surface = surface;

		//Set the XDG surface.
		window_data->_WaylandWindowData._XDGSurface = xdg_surface;

		//Set the XDG top level.
		window_data->_WaylandWindowData._XDGTopLevel = xdg_top_level;

		//Set the decoration.
		window_data->_WaylandWindowData._Decoration = decoration;

		//Add the surface listener.
		xdg_surface_add_listener
		(
			window_data->_WaylandWindowData._XDGSurface,
			&XDG_SURFACE_LISTENER,
			window_data
		);

		//Add the top level listener.
		xdg_toplevel_add_listener
		(
			window_data->_WaylandWindowData._XDGTopLevel,
			&XDG_TOP_LEVEL_LISTENER,
			window_data
		);

		//Set the title.
		xdg_toplevel_set_title(window_data->_WaylandWindowData._XDGTopLevel, parameters._Title.Data());

		//Set the preferred resolution.
		xdg_toplevel_set_min_size(window_data->_WaylandWindowData._XDGTopLevel, parameters._Resolution._Width, parameters._Resolution._Height);
		xdg_toplevel_set_max_size(window_data->_WaylandWindowData._XDGTopLevel, parameters._Resolution._Width, parameters._Resolution._Height);

		//Set fullscreen.
		if (parameters._FullScreen)
		{
			xdg_toplevel_set_fullscreen(window_data->_WaylandWindowData._XDGTopLevel, nullptr);
		}

		//Do the initial commit.
		wl_surface_commit(window_data->_WaylandWindowData._Surface);

		//Wait for the window to get configured.
		while (!window_data->_WaylandWindowData._Configured)
		{
			if (wl_display_dispatch(_WaylandContext._Display) == -1)
			{
				delete window_data;

				return Platform::INVALID_WINDOW_HANDLE;
			}
		}

	#if defined(CATALYST_HAS_OPEN_GL)
		//If this window is being asked to support OpenGL, set that up.
		if (parameters._SupportOpenGL)
		{
			window_data->_SupportsOpenGL = CreateOpenGLWindow(parameters, window_data);
		}
	#endif

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
		//Return the display.
		return _WaylandContext._Display;
	}

	/*
	*	Returns a platform native surface handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
	*/
	NO_DISCARD Platform::PlatformNativeWindowHandle GetSurfaceHandle(const Platform::WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Return the surface.
		return window_data->_WaylandWindowData._Surface;
	}

	/*
	*	Prepares OpenGL for the given window.
	*/
	void PrepareOpenGL(const Platform::WindowHandle window) NOEXCEPT
	{
	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Set the current context.
		eglMakeCurrent
		(
			window_data->_WaylandWindowData._EGLDisplay,
			window_data->_WaylandWindowData._EGLSurface,
			window_data->_WaylandWindowData._EGLSurface,
			window_data->_WaylandWindowData._EGLContext
		);
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

		//Swap the buffers.
		eglSwapBuffers(window_data->_WaylandWindowData._EGLDisplay, window_data->_WaylandWindowData._EGLSurface);
	#endif
	}

    /*
	*	Destroys the given window.
	*/
	void Destroy(const Platform::WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Destroy the decoration.
		if (window_data->_WaylandWindowData._Decoration)
		{
			zxdg_toplevel_decoration_v1_destroy(window_data->_WaylandWindowData._Decoration);
		}

		//Destroy the XDG top level.
		xdg_toplevel_destroy(window_data->_WaylandWindowData._XDGTopLevel);

		//Destroy the XDG surface.
		xdg_surface_destroy(window_data->_WaylandWindowData._XDGSurface);

		//Destroy the surface.
		wl_surface_destroy(window_data->_WaylandWindowData._Surface);

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

    //The Wayland context.
    WaylandContext _WaylandContext;

#if defined(CATALYST_HAS_OPEN_GL)
    /*
    *	Creates an OpenGL window for the given window data. Returns if it succeeded.
    */
    FORCE_INLINE NO_DISCARD bool CreateOpenGLWindow
    (
        const Platform::WindowManager::CreateParameters &parameters,
        WindowData *const RESTRICT window_data
    ) NOEXCEPT
    {
        //Create the EGL window.
        window_data->_WaylandWindowData._EGLWindow = wl_egl_window_create(window_data->_WaylandWindowData._Surface, parameters._Resolution._Width, parameters._Resolution._Height);

        if (!window_data->_WaylandWindowData._EGLWindow)
        {
            return false;
        }

        //Create the EGL display.
        window_data->_WaylandWindowData._EGLDisplay = eglGetDisplay(_WaylandContext._Display);

        if (window_data->_WaylandWindowData._EGLDisplay == EGL_NO_DISPLAY)
        {
            return false;
        }

        //Initialize EGL.
        EGLint major_version;
        EGLint minor_version;

        if (!eglInitialize(window_data->_WaylandWindowData._EGLDisplay, &major_version, &minor_version))
        {
            return false;
        }

        LOG_INFORMATION("Initialized EGL with version %i.%i", major_version, minor_version);

        //Choose the config.
        constexpr EGLint CONFIG_ATTRIBUTES[]
        {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,

            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,

            EGL_DEPTH_SIZE, 24,
            EGL_STENCIL_SIZE, 8,

            EGL_NONE
        };

        EGLConfig chosen_config;
        EGLint number_of_configs;

        if
        (	!eglChooseConfig
            (
                window_data->_WaylandWindowData._EGLDisplay,
                CONFIG_ATTRIBUTES,
                &chosen_config,
                1,
                &number_of_configs
            )
            ||
            number_of_configs == 0
        )
        {
            return false;
        }

        //Bind the API.
        if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
        {
            return false;
        }

        //Create the EGL context.
        constexpr EGLint CONTEXT_ATTRIBUTES[]
        {
            EGL_CONTEXT_MAJOR_VERSION, 3,
            EGL_CONTEXT_MINOR_VERSION, 3,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,

            EGL_NONE
        };

        window_data->_WaylandWindowData._EGLContext = eglCreateContext
        (
            window_data->_WaylandWindowData._EGLDisplay,
            chosen_config,
            EGL_NO_CONTEXT,
            CONTEXT_ATTRIBUTES
        );

        if (window_data->_WaylandWindowData._EGLContext == EGL_NO_CONTEXT)
        {
            return false;
        }

        //Create the EGL surface.
        window_data->_WaylandWindowData._EGLSurface = eglCreateWindowSurface
        (
            window_data->_WaylandWindowData._EGLDisplay,
            chosen_config,
            reinterpret_cast<NativeWindowType>(window_data->_WaylandWindowData._EGLWindow),
            nullptr
        );

        if (window_data->_WaylandWindowData._EGLSurface == EGL_NO_SURFACE)
        {
            return false;
        }

        //Set the current context.
        if
        (
            !eglMakeCurrent
            (
                window_data->_WaylandWindowData._EGLDisplay,
                window_data->_WaylandWindowData._EGLSurface,
                window_data->_WaylandWindowData._EGLSurface,
                window_data->_WaylandWindowData._EGLContext
            )
        )
        {
            return false;
        }

        //Retrieve the function pointers for OpenGL.
        OpenGL::RetrieveOpenGLFunctionPointers();

        return true;
    }
    #endif

};