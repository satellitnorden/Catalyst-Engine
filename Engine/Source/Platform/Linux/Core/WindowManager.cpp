#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Platform/Core/WindowManager.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/SpinLock.h>

//Third party.
#include <Wayland/xdg-shell-client-protocol.h>

//STD.
#include <cstring>

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

	/*
	*	Initializes this Wayland context.
	*	Returns if the initialization succeded.
	*/
	FORCE_INLINE NO_DISCARD bool Initialize() NOEXCEPT
	{
		//Define constants.
		constexpr wl_registry_listener REGISTRY_LISTENER
		{
			RegistryListen,
			RegistryRemove
		};
		constexpr xdg_wm_base_listener XDG_WINDOW_MANAGER_BASE_LISTENER
		{
			XDGWindowManagerPing
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

		//Add the XDG window manager listener.
		if (xdg_wm_base_add_listener(_XDGWindowManager, &XDG_WINDOW_MANAGER_BASE_LISTENER, this) != 0)
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

};

/*
*	Window data class definition.
*/
class WindowData final
{

public:

	//The surface.
	wl_surface *RESTRICT _Surface{ nullptr };

	//The XDG surface.
	xdg_surface *RESTRICT _XDGSurface{ nullptr };

	//The XDG top level.
	xdg_toplevel *RESTRICT _XDGTopLevel{ nullptr };

	//Denotes whether or not this window is focused.
	bool _Focused{ false };

	//The callbacks.
	Platform::WindowManager::Callbacks _Callbacks;

	//The last mouse position.
	Vector2<float32> _LastMousePosition{ 0.5f, 0.5f };

};

//The wayland context.
WaylandContext _WaylandContext;

//The window data lock.
SpinLock _WindowDataLock;

//The window data.
DynamicArray<WindowData *RESTRICT> _WindowData;

//The master window data.
WindowData *RESTRICT _MasterWindowData{ nullptr };

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
	*/
	void WindowManager::Update() NOEXCEPT
	{
		
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

		//Allocate the window data.
		WindowData *const RESTRICT window_data{ new WindowData() };

		//Set the surface.
		window_data->_Surface = surface;

		//Set the XDG surface.
		window_data->_XDGSurface = xdg_surface;

		//Set the XDG top level.
		window_data->_XDGTopLevel = xdg_top_level;

		//Add the window data.
		{
			SCOPED_LOCK(_WindowDataLock);
			_WindowData.Emplace(window_data);
		}

		//Return the window data!
		return window_data;
	}

	/*
	*	Returns the platform native handle for the given window handle.
	*	May or may not be the same as the window handle, depending on the platform.
	*/
	NO_DISCARD WindowHandle WindowManager::GetPlatformNativeHandle(const WindowHandle window) NOEXCEPT
	{
		return INVALID_WINDOW_HANDLE;
	}

	/*
	*	Returns if the given window is alive.
	*/
	NO_DISCARD bool WindowManager::IsAlive(const WindowHandle window) NOEXCEPT
	{
		return true;
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
	*	Adds a keyboard button up callback to the given window.
	*/
	void WindowManager::AddKeyboardButtonUpCallback(const WindowHandle window, const KeyboardButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT
	{
		//Cache the window data.
		WindowData *const RESTRICT window_data{ static_cast<WindowData *const RESTRICT>(window) };

		//Add the callback.
		window_data->_Callbacks.AddKeyboardButtonUpCallback(callback, user_data);
	}

}
#endif