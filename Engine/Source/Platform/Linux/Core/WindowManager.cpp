#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/Core/WindowManager.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>

//Rendering.
#include <Rendering/Abstraction/OpenGL/OpenGL.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <Wayland/xdg-shell-client-protocol.h>
#include <Wayland/xdg-decoration-unstable-v1-client-protocol.h>
#include <linux/input-event-codes.h>
#if defined(CATALYST_HAS_OPEN_GL)
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
#endif
#include <errno.h>
#include <poll.h>
#if defined(CATALYST_HAS_OPEN_GL)
	#include <wayland-egl.h>
#endif

//STD.
#include <cstring>

/*
*	Window data class definition.
*/
class WindowData final
{

public:

	//The parameters.
	Platform::WindowManager::CreateParameters _Parameters;

	//The surface.
	wl_surface *RESTRICT _Surface{ nullptr };

	//The XDG surface.
	xdg_surface *RESTRICT _XDGSurface{ nullptr };

	//The XDG top level.
	xdg_toplevel *RESTRICT _XDGTopLevel{ nullptr };

	//The decoration.
	zxdg_toplevel_decoration_v1 *RESTRICT _Decoration{ nullptr };

#if defined(CATALYST_HAS_OPEN_GL)
	//The EGL window.
	wl_egl_window *RESTRICT _EGLWindow{ nullptr };

	//The EGL display.
	EGLDisplay _EGLDisplay;

	//The EGL context.
	EGLContext _EGLContext;

	//The EGL surface.
	EGLSurface _EGLSurface;
#endif

	//Denotes whether or not this window supports OpenGL.
	bool _SupportsOpenGL{ false };

	//The final resolution.
	Resolution _FinalResolution;

	//Denotes whether or not this window is configured yet.
	bool _Configured{ false };

	//Denotes whether or not the mouse has entered this window.
	bool _MouseEntered{ false };

	//Denotes whether or not this window is focused.
	bool _Focused{ false };

	//The callbacks.
	Platform::WindowManager::Callbacks _Callbacks;

	//The last mouse position.
	Vector2<float32> _LastMousePosition{ 0.5f, 0.5f };

	//The timer identifier generator.
	uint64 _TimerIdentifierGenerator{ 0 };

};

//The window data lock.
SpinLock _WindowDataLock;

//The window data.
DynamicArray<WindowData *RESTRICT> _WindowData;

//The master window data.
WindowData *RESTRICT _MasterWindowData{ nullptr };

/*
*	Wayland context class definition.
*/
class WaylandContext final
{

public:

	//Denotes whether or not this Wayland context is initialized.
	bool _Initialized{ false };

	//The display.
	wl_display *RESTRICT _Display{ nullptr };

	//The registry.
	wl_registry *RESTRICT _Registry{ nullptr };

	//The compositor.
	wl_compositor *RESTRICT _Compositor{ nullptr };

	//The XDG window manager.
	xdg_wm_base *RESTRICT _XDGWindowManager{ nullptr };

	//The seat.
	wl_seat *RESTRICT _Seat{ nullptr };

	//The pointer.
	wl_pointer *RESTRICT _Pointer{ nullptr };

	//The ZXDG decoration manager.
	zxdg_decoration_manager_v1 *RESTRICT _ZXDGDecorationManager{ nullptr };

	/*
	*	Initializes this Wayland context.
	*	Returns if the initialization succeded.
	*/
	FORCE_INLINE NO_DISCARD bool Initialize() NOEXCEPT
	{
		//Define constants.
		static constexpr wl_registry_listener REGISTRY_LISTENER
		{
			RegistryListen,
			RegistryRemove
		};

		//Skip if we're already initialized.
		if (_Initialized)
		{
			return true;
		}

		//Retrieve the display.
		_Display = wl_display_connect(nullptr);

		if (!_Display)
		{
			Clean();

			return false;
		}

		//Retrieve the registry.
		_Registry = wl_display_get_registry(_Display);

		if (!_Registry)
		{
			Clean();

			return false;
		}

		//Add the registry listener.
		if (wl_registry_add_listener(_Registry, &REGISTRY_LISTENER, this) != 0)
		{
			Clean();

			return false;
		}

		//Do the display roundtrip to call the registry listener.
		if (wl_display_roundtrip(_Display) < 0)
		{
			Clean();

			return false;
		}

		//Check that we have the required interfaces.
		if (!_Compositor || !_XDGWindowManager)
		{
			Clean();

			return false;
		}

		//We are now initialized!
		_Initialized = true;

		return true;
	}

private:

	/*
	*	Cleans.
	*/
	FORCE_INLINE void Clean() NOEXCEPT
	{
		if (_XDGWindowManager)
		{
			xdg_wm_base_destroy(_XDGWindowManager);
			_XDGWindowManager = nullptr;
		}

		if (_Compositor)
		{
			wl_compositor_destroy(_Compositor);
			_Compositor = nullptr;
		}

		if (_Registry)
		{
			wl_registry_destroy(_Registry);
			_Registry = nullptr;
		}

		if (_Display)
		{
			wl_display_disconnect(_Display);
			_Display = nullptr;
		}
	}

	/*
	*	The registry listen function.
	*/
	FORCE_INLINE static void RegistryListen
	(
		void *data,
		wl_registry *registry,
		uint32_t name,
		const char *interface,
		uint32_t version
	)
	{
		//Define constants.
		static constexpr xdg_wm_base_listener XDG_WINDOW_MANAGER_BASE_LISTENER
		{
			XDGWindowManagerPing
		};
		static constexpr wl_seat_listener SEAT_LISTENER
		{
			SeatListenerCapabilities,
			SeatListenerName
		};

		//Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		if (strcmp(interface, wl_compositor_interface.name) == 0)
		{
			PRINT_TO_OUTPUT("Found 'wl_compositor' interface!");

			context->_Compositor = static_cast<wl_compositor *const RESTRICT>
			(
				wl_registry_bind
				(
					registry,
					name,
					&wl_compositor_interface,
					std::min(version, static_cast<uint32_t>(wl_compositor_interface.version))
				)
			);
		}

		else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
		{
			PRINT_TO_OUTPUT("Found 'xdg_wm_base' interface!");

			context->_XDGWindowManager = static_cast<xdg_wm_base *const RESTRICT>
			(
				wl_registry_bind
				(
					registry,
					name,
					&xdg_wm_base_interface,
					std::min(version, static_cast<uint32_t>(xdg_wm_base_interface.version))
				)
			);

			xdg_wm_base_add_listener(context->_XDGWindowManager, &XDG_WINDOW_MANAGER_BASE_LISTENER, context);
		}

		else if (strcmp(interface, wl_seat_interface.name) == 0)
		{
			PRINT_TO_OUTPUT("Found 'wl_seat_interface' interface!");

			context->_Seat = static_cast<wl_seat *const RESTRICT>
			(
				wl_registry_bind
				(
					registry,
					name,
					&wl_seat_interface,
					std::min(version, static_cast<uint32_t>(wl_seat_interface.version))
				)
			);

			wl_seat_add_listener(context->_Seat, &SEAT_LISTENER, context);
		}

		else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
		{
			PRINT_TO_OUTPUT("Found 'zxdg_decoration_manager_v1_interface' interface!");

			context->_ZXDGDecorationManager = static_cast<zxdg_decoration_manager_v1 *const RESTRICT>
			(
				wl_registry_bind
				(
					registry,
					name,
					&zxdg_decoration_manager_v1_interface,
					std::min(version, static_cast<uint32_t>(zxdg_decoration_manager_v1_interface.version))
				)
			);
		}
	}

	/*
	*	The registry remove function.
	*/
	FORCE_INLINE static void RegistryRemove
	(
		void *data,
		wl_registry *registry,
		uint32_t name
	)
	{
		
	}

	/*
	*	The XDG Window Manager ping function.
	*/
	static void XDGWindowManagerPing
	(
		void *data,
		xdg_wm_base *xdg_window_manager,
		uint32_t serial
	)
	{
		xdg_wm_base_pong(xdg_window_manager, serial);
	}

	/*
	*	The seat listener capabilities function.
	*/
	static void SeatListenerCapabilities
	(
		void *data,
		wl_seat *seat,
		uint capabilities
	)
	{
		//Define constants.
		static constexpr wl_pointer_listener POINTER_LISTENER
		{
			PointerListenerEnter,
			PointerListenerLeave,
			PointerListenerMotion,
			PointerListenerButton,
			PointerListenerAxis,
			PointerListenerFrame,
			PointerListenerAxisSource,
			PointerListenerAxisStop,
			PointerListenerAxisDiscrete
		};

		//Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		//Check if this seat has a pointer capability.
		if (TEST_BIT(capabilities, WL_SEAT_CAPABILITY_POINTER))
		{
			//Create the pointer.
			if (!context->_Pointer)
			{
				context->_Pointer = wl_seat_get_pointer(seat);

				if (context->_Pointer)
				{
					if (wl_pointer_add_listener(context->_Pointer, &POINTER_LISTENER, context) < 0)
					{
						wl_pointer_release(context->_Pointer);
						context->_Pointer = nullptr;
					}
				}
			}
		}

		else
		{
			//Destroy the pointer.
			if (context->_Pointer)
			{
				wl_pointer_release(context->_Pointer);
				context->_Pointer = nullptr;
			}
		}
	}

	/*
	*	The seat listener name function.
	*/
	static void SeatListenerName
	(
		void *data,
		wl_seat *seat,
		const char *name
	)
	{

	}

	/*
	*	The pointer listener enter function.
	*/
	static void PointerListenerEnter
	(
		void *data,
		wl_pointer *pointer,
		uint serial,
		wl_surface *surface,
		wl_fixed_t surface_x,
		wl_fixed_t surface_y
	)
	{
		for (WindowData *const RESTRICT window_data : _WindowData)
		{
			if (window_data->_Surface == surface)
			{
				window_data->_MouseEntered = true;
			}
		}
	}

	/*
	*	The pointer listener leave function.
	*/
	static void PointerListenerLeave
	(
		void *data,
		wl_pointer *pointer,
		uint serial,
		wl_surface *surface
	)
	{
		for (WindowData *const RESTRICT window_data : _WindowData)
		{
			if (window_data->_Surface == surface)
			{
				window_data->_MouseEntered = false;
			}
		}
	}

	/*
	*	The pointer listener motion function.
	*/
	static void PointerListenerMotion
	(
		void *data,
		wl_pointer *pointer,
		uint time,
		wl_fixed_t surface_x,
		wl_fixed_t surface_y
	)
	{
		//Find the window data that the mouse has entered - Presumably that should only be one.
		WindowData *RESTRICT window_data{ nullptr };

		for (WindowData *const RESTRICT _window_data : _WindowData)
		{
			if (_window_data->_MouseEntered)
			{
				window_data = _window_data;

				break;
			}
		}

		if (!window_data)
		{
			return;
		}

		//Calculate the mouse position.
		const Vector2<float32> mouse_position
		{
			static_cast<float32>(wl_fixed_to_double(surface_x)) / static_cast<float32>(window_data->_FinalResolution._Width),
			1.0f - static_cast<float32>(wl_fixed_to_double(surface_y)) / static_cast<float32>(window_data->_FinalResolution._Height)
		};

		//Call the callbacks if the mouse position changed.
		if (window_data->_LastMousePosition != mouse_position)
		{
			window_data->_Callbacks.CallMouseMoveCallbacks(mouse_position);
			window_data->_LastMousePosition = mouse_position;
		}
	}

	/*
	*	The pointer listener button function.
	*/
	static void PointerListenerButton
	(
		void *data,
		wl_pointer *pointer,
		uint serial,
		uint time,
		uint button,
		uint state
	)
	{
		//Find the window data that the mouse has entered - Presumably that should only be one.
		WindowData *RESTRICT window_data{ nullptr };

		for (WindowData *const RESTRICT _window_data : _WindowData)
		{
			if (_window_data->_MouseEntered)
			{
				window_data = _window_data;

				break;
			}
		}

		if (!window_data)
		{
			return;
		}

		//Call the callbacks.
		switch (button)
		{
			case BTN_LEFT:
			{
				switch (state)
				{
					case WL_POINTER_BUTTON_STATE_PRESSED:
					{
						window_data->_Callbacks.CallMouseButtonDownCallbacks(MouseButton::LEFT);

						break;
					}

					case WL_POINTER_BUTTON_STATE_RELEASED:
					{
						window_data->_Callbacks.CallMouseButtonUpCallbacks(MouseButton::LEFT);

						break;
					}
				}

				break;
			}

			case BTN_MIDDLE:
			{
				switch (state)
				{
					case WL_POINTER_BUTTON_STATE_PRESSED:
					{
						window_data->_Callbacks.CallMouseButtonDownCallbacks(MouseButton::MIDDLE);

						break;
					}

					case WL_POINTER_BUTTON_STATE_RELEASED:
					{
						window_data->_Callbacks.CallMouseButtonUpCallbacks(MouseButton::MIDDLE);

						break;
					}
				}

				break;
			}

			case BTN_RIGHT:
			{
				switch (state)
				{
					case WL_POINTER_BUTTON_STATE_PRESSED:
					{
						window_data->_Callbacks.CallMouseButtonDownCallbacks(MouseButton::RIGHT);

						break;
					}

					case WL_POINTER_BUTTON_STATE_RELEASED:
					{
						window_data->_Callbacks.CallMouseButtonUpCallbacks(MouseButton::RIGHT);

						break;
					}
				}

				break;
			}
		}
	}

	/*
	*	The pointer listener axis function.
	*/
	static void PointerListenerAxis
	(
		void *data,
		wl_pointer *wl_pointer,
		uint tie,
		uint axis,
		wl_fixed_t value
	)
	{

	}

	/*
	*	The poointer listener frame function.
	*/
	static void PointerListenerFrame
	(
		void *data,
		wl_pointer *pointer
	)
	{

	}

	/*
	*	The pointer listener axis source function.
	*/
	static void PointerListenerAxisSource
	(
		void *data,
		wl_pointer *pointer,
		uint axis_source
	)
	{

	}

	/*
	*	The pointer listener axis stop function.
	*/
	static void PointerListenerAxisStop
	(
		void *data,
		wl_pointer *pointer,
		uint time,
		uint axis
	)
	{

	}

	/*
	*	The pointer listener axis discrete function.
	*/
	static void PointerListenerAxisDiscrete
	(
		void *data,
		wl_pointer *pointer,
		uint axis,
		int discrete
	)
	{

	}

};

//The wayland context.
WaylandContext _WaylandContext;

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
	window_data->_Configured = true;
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
	window_data->_EGLWindow = wl_egl_window_create(window_data->_Surface, parameters._Resolution._Width, parameters._Resolution._Height);

	if (!window_data->_EGLWindow)
	{
		return false;
	}

	//Create the EGL display.
	window_data->_EGLDisplay = eglGetDisplay(_WaylandContext._Display);

	if (window_data->_EGLDisplay == EGL_NO_DISPLAY)
	{
		return false;
	}

	//Initialize EGL.
	EGLint major_version;
	EGLint minor_version;

	if (!eglInitialize(window_data->_EGLDisplay, &major_version, &minor_version))
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
			window_data->_EGLDisplay,
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

	window_data->_EGLContext = eglCreateContext
	(
		window_data->_EGLDisplay,
		chosen_config,
		EGL_NO_CONTEXT,
		CONTEXT_ATTRIBUTES
	);

	if (window_data->_EGLContext == EGL_NO_CONTEXT)
	{
		return false;
	}

	//Create the EGL surface.
	window_data->_EGLSurface = eglCreateWindowSurface
	(
		window_data->_EGLDisplay,
		chosen_config,
		reinterpret_cast<NativeWindowType>(window_data->_EGLWindow),
		nullptr
	);

	if (window_data->_EGLSurface == EGL_NO_SURFACE)
	{
		return false;
	}

	//Set the current context.
	if
	(
		!eglMakeCurrent
		(
			window_data->_EGLDisplay,
			window_data->_EGLSurface,
			window_data->_EGLSurface,
			window_data->_EGLContext
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
		//Initialize the Wayland context.
		if (!_WaylandContext.Initialize())
		{
			return INVALID_WINDOW_HANDLE;
		}

		//Create the surface.
		wl_surface *const RESTRICT surface{ wl_compositor_create_surface(_WaylandContext._Compositor) };

		if (!surface)
		{
			return INVALID_WINDOW_HANDLE;
		}

		//Create the XDG surface.
		xdg_surface *const RESTRICT xdg_surface{ xdg_wm_base_get_xdg_surface(_WaylandContext._XDGWindowManager, surface) };

		if (!xdg_surface)
		{
			wl_surface_destroy(surface);

			return INVALID_WINDOW_HANDLE;
		}

		//Create the XDG top level.
		xdg_toplevel *const RESTRICT xdg_top_level{ xdg_surface_get_toplevel(xdg_surface) };

		if (!xdg_top_level)
		{
			xdg_surface_destroy(xdg_surface);
			wl_surface_destroy(surface);

			return INVALID_WINDOW_HANDLE;
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
		window_data->_Surface = surface;

		//Set the XDG surface.
		window_data->_XDGSurface = xdg_surface;

		//Set the XDG top level.
		window_data->_XDGTopLevel = xdg_top_level;

		//Set the decoration.
		window_data->_Decoration = decoration;

		//Add the surface listener.
		xdg_surface_add_listener
		(
			window_data->_XDGSurface,
			&XDG_SURFACE_LISTENER,
			window_data
		);

		//Add the top level listener.
		xdg_toplevel_add_listener
		(
			window_data->_XDGTopLevel,
			&XDG_TOP_LEVEL_LISTENER,
			window_data
		);

		//Set the title.
		xdg_toplevel_set_title(window_data->_XDGTopLevel, parameters._Title.Data());

		//Set the preferred resolution.
		xdg_toplevel_set_min_size(window_data->_XDGTopLevel, parameters._Resolution._Width, parameters._Resolution._Height);
		xdg_toplevel_set_max_size(window_data->_XDGTopLevel, parameters._Resolution._Width, parameters._Resolution._Height);

		//Set fullscreen.
		if (parameters._FullScreen)
		{
			xdg_toplevel_set_fullscreen(window_data->_XDGTopLevel, nullptr);
		}

		//Do the initial commit.
		wl_surface_commit(window_data->_Surface);

		//Wait for the window to get configured.
		while (!window_data->_Configured)
		{
			if (wl_display_dispatch(_WaylandContext._Display) == -1)
			{
				delete window_data;

				return INVALID_WINDOW_HANDLE;
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
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetWindowHandle(const WindowHandle window) NOEXCEPT
	{
		//Linux does not understand the concept of a "window" handle.
		return INVALID_WINDOW_HANDLE;
	}

	/*
	*	Returns a platform native display handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetDisplayHandle(const WindowHandle window) NOEXCEPT
	{
		//Return the display.
		return _WaylandContext._Display;
	}

	/*
	*	Returns a platform native surface handle for the given window.
	*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
	*/
	NO_DISCARD PlatformNativeWindowHandle WindowManager::GetSurfaceHandle(const WindowHandle window) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Return the surface.
		return window_data->_Surface;
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

	#if defined(CATALYST_HAS_OPEN_GL)
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Set the current context.
		eglMakeCurrent
		(
			window_data->_EGLDisplay,
			window_data->_EGLSurface,
			window_data->_EGLSurface,
			window_data->_EGLContext
		);
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
		eglSwapBuffers(window_data->_EGLDisplay, window_data->_EGLSurface);
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

		//Destroy the decoration.
		if (window_data->_Decoration)
		{
			zxdg_toplevel_decoration_v1_destroy(window_data->_Decoration);
		}

		//Destroy the XDG top level.
		xdg_toplevel_destroy(window_data->_XDGTopLevel);

		//Destroy the XDG surface.
		xdg_surface_destroy(window_data->_XDGSurface);

		//Destroy the surface.
		wl_surface_destroy(window_data->_Surface);

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