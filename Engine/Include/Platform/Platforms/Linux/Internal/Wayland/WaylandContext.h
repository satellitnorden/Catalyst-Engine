#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Platforms/Linux/Internal/Wayland/WaylandCore.h>
#include <Platform/Platforms/Linux/Internal/Windows/WindowData.h>

//Third party.
#include <linux/input-event-codes.h>

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

	//The seat.
	wl_seat *RESTRICT _Seat{ nullptr };

	//The pointer.
	wl_pointer *RESTRICT _Pointer{ nullptr };

	//The ZXDG decoration manager.
	zxdg_decoration_manager_v1 *RESTRICT _ZXDGDecorationManager{ nullptr };

	/*
	*	Default constructor.
	*/
	FORCE_INLINE WaylandContext(DynamicArray<WindowData *RESTRICT> &window_data) NOEXCEPT
		:
		_WindowData(window_data)
	{
		
	}

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

	//The window data.
	DynamicArray<WindowData *RESTRICT> &_WindowData;

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
		//Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		for (WindowData *const RESTRICT window_data : context->_WindowData)
		{
			if (window_data->_WaylandWindowData._Surface == surface)
			{
				window_data->_WaylandWindowData._MouseEntered = true;
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
        //Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		for (WindowData *const RESTRICT window_data : context->_WindowData)
		{
			if (window_data->_WaylandWindowData._Surface == surface)
			{
				window_data->_WaylandWindowData._MouseEntered = false;
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
        //Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		//Find the window data that the mouse has entered - Presumably that should only be one.
		WindowData *RESTRICT window_data{ nullptr };

		for (WindowData *const RESTRICT _window_data : context->_WindowData)
		{
			if (_window_data->_WaylandWindowData._MouseEntered)
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
        //Cache the context.
		WaylandContext *const RESTRICT context{ static_cast<WaylandContext *const RESTRICT>(data) };

		//Find the window data that the mouse has entered - Presumably that should only be one.
		WindowData *RESTRICT window_data{ nullptr };

		for (WindowData *const RESTRICT _window_data : context->_WindowData)
		{
			if (_window_data->_WaylandWindowData._MouseEntered)
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