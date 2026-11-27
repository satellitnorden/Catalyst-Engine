#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>
#include <Core/General/StaticString.h>

//Input.
#include <Input/InputCore.h>

//Math.
#include <Math/General/Vector.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

namespace Platform
{

	//Type aliases.
	using WindowHandle = void *RESTRICT;
	using PlatformNativeWindowHandle = void *RESTRICT;

	//Defines and invalid window handle.
	constexpr WindowHandle INVALID_WINDOW_HANDLE{ nullptr };

	/*
	*	Window manager class defintion.
	*	Each supported platform should implement this interfacein a .cpp file.
	*/
	class WindowManager final
	{

	public:

		//Enumeration covering all backends.
		enum class Backend : uint8
		{
		#if defined(CATALYST_PLATFORM_WINDOWS)
			WIN32,

			DEFAULT = WIN32
		#elif defined(CATALYST_PLATFORM_LINUX)
			WAYLAND,
			X11,

			DEFAULT = WAYLAND
		#else
			#error "Unknown platform!"
		#endif
		};

		/*
		*	Initializes the window manager.
		*/
		static void Initialize() NOEXCEPT;

		/*
		*	Updates the window manager.
		*	Can optionally specify a specific window handle to update, otherwise will update all managed windows.
		*/
		static void Update(const WindowHandle window) NOEXCEPT;

		/*
		*	Terminates the window manager.
		*/
		static void Terminate() NOEXCEPT;

		/*
		*	Create parameters class definition.
		*/
		class CreateParameters final
		{

		public:

			//The backend.
			Backend _Backend{ Backend::DEFAULT };

			//The title.
			StaticString<128> _Title;

			//The resolution.
			Resolution _Resolution;

			//The parent.
			PlatformNativeWindowHandle _Parent{ INVALID_WINDOW_HANDLE };

			//Denotes whether or not this window is fullscreen.
			bool _FullScreen;

			/*
			*	Denotes whether or not the resolution includes platform elements, such as the title bar etc.
			*	If set to false, the actual drawing area of the window keeps the exact provided resolution,
			*	in which case the resulting total size of the window may be larger than the provided resolution.
			*/
			bool _ResolutionIncludesPlatformElements{ false };

			/*
			*	Denotes whether or not this window needs to support OpenGL.
			*	On some platforms windows needs to be created in a specific way to support OpenGL,
			*	which is why this needs to be specified when creating a window.
			*/
			bool _SupportOpenGL{ false };

		};

		/*
		*	Creates a window with the given parameters.
		*/
		static NO_DISCARD WindowHandle Create(const CreateParameters &parameters) NOEXCEPT;

		/*
		*	Returns the backend of the given window.
		*/
		static NO_DISCARD Backend GetBackend(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns a platform native window handle for the given window.
		*	Not all platforms may return a valid handle here, as the concept of a "window" may differ between platforms.
		*/
		static NO_DISCARD PlatformNativeWindowHandle GetWindowHandle(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns a platform native display handle for the given window.
		*	Not all platforms may return a valid handle here, as the concept of a "display" may differ between platforms.
		*/
		static NO_DISCARD PlatformNativeWindowHandle GetDisplayHandle(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns a platform native surface handle for the given window.
		*	Not all platforms may return a valid handle here, as the concept of a "surface" may differ between platforms.
		*/
		static NO_DISCARD PlatformNativeWindowHandle GetSurfaceHandle(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns the resolution of the given window.
		*	The resolution passed in to 'Create()' may not be the final resolution of the window,
		*	due to some platforms making other decisions,
		*	so call this to query the actual resolution of the renderable area of the window.
		*/
		static NO_DISCARD Resolution GetResolution(const WindowHandle window) NOEXCEPT;

		/*
		*	Sets the resolution of the given window.
		*/
		static void SetResolution(const WindowHandle window, const Resolution &resolution) NOEXCEPT;

		/*
		*	Returns if the given window is alive.
		*/
		static NO_DISCARD bool IsAlive(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns if the given window is focused.
		*/
		static NO_DISCARD bool IsFocused(const WindowHandle window) NOEXCEPT;

		/*
		*	Shows the given window.
		*/
		static void Show(const WindowHandle window) NOEXCEPT;

		/*
		*	Hides the given window.
		*/
		static void Hide(const WindowHandle window) NOEXCEPT;

		/*
		*	Renders to the given window with the given data.
		*/
		static void Render(const WindowHandle window, const uint32 *const RESTRICT data) NOEXCEPT;

		/*
		*	Returns if the given window supports OpenGL.
		*/
		static NO_DISCARD bool SupportsOpenGL(const WindowHandle window) NOEXCEPT;

		/*
		*	Prepares OpenGL for the given window.
		*/
		static void PrepareOpenGL(const WindowHandle window) NOEXCEPT;

		/*
		*	Finalizes OpenGL for the given window.
		*/
		static void FinalizeOpenGL(const WindowHandle window) NOEXCEPT;

		/*
		*	Returns the master window.
		*/
		static NO_DISCARD const WindowHandle GetMasterWindow() NOEXCEPT;

		/*
		*	Sets the master window.
		*/
		static void SetMasterWindow(const WindowHandle window) NOEXCEPT;

		/*
		*	Destroys the given window.
		*/
		static void Destroy(const WindowHandle window) NOEXCEPT;

		//Type aliases.
		using WindowClosedCallback = void(*)(void *const RESTRICT user_data);
		using KeyboardButtonDownCallback = void(*)(const KeyboardButton keyboard_button, void *const RESTRICT user_data);
		using KeyboardButtonUpCallback = void(*)(const KeyboardButton keyboard_button, void *const RESTRICT user_data);
		using MouseMoveCallback = void(*)(const Vector2<float32> position, void *const RESTRICT user_data);
		using MouseButtonDownCallback = void(*)(const MouseButton mouse_button, void *const RESTRICT user_data);
		using MouseButtonUpCallback = void(*)(const MouseButton mouse_button, void *const RESTRICT user_data);
		using MouseScrollWheelCallback = void(*)(const int8 step, void *const RESTRICT user_data);
		using TimerCallback = void(*)(void *const RESTRICT user_data);

		/*
		*	Adds window closed callback to the given window.
		*/
		static void AddWindowClosedCallback(const WindowHandle window, const WindowClosedCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a keyboard button down callback to the given window.
		*/
		static void AddKeyboardButtonDownCallback(const WindowHandle window, const KeyboardButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a keyboard button up callback to the given window.
		*/
		static void AddKeyboardButtonUpCallback(const WindowHandle window, const KeyboardButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a mouse move callback to the given window.
		*/
		static void AddMouseMoveCallback(const WindowHandle window, const MouseMoveCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a mouse button down callback to the given window.
		*/
		static void AddMouseButtonDownCallback(const WindowHandle window, const MouseButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a mouse button up callback to the given window.
		*/
		static void AddMouseButtonUpCallback(const WindowHandle window, const MouseButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a mouse scroll wheel callback to the given window.
		*/
		static void AddMouseScrollWheelCallback(const WindowHandle window, const MouseScrollWheelCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Adds a timer callback to the given window.
		*	Frequency is denoted in milliseconds.
		*	Returns an identifier that can be used to remove the timer later.
		*/
		static NO_DISCARD uint64 AddTimerCallback(const WindowHandle window, const uint64 frequency, const TimerCallback callback, void *const RESTRICT user_data) NOEXCEPT;

		/*
		*	Removes the timer with the given identifier from the given window.
		*/
		static void RemoveTimerCallback(const WindowHandle window, const uint64 identifier) NOEXCEPT;

		/*
		*	Callbacks class definition.
		*	Utility class for holding per-window callbacks and calling them.
		*/
		class Callbacks final
		{

		public:

			//The window closed callbacks.
			DynamicArray<WindowClosedCallback> _WindowClosedCallbacks;

			//The window closed callback user data.
			DynamicArray<void *RESTRICT> _WindowClosedCallbackUserData;

			//The number of window closed callbacks.
			uint64 _NumberOfWindowClosedCallbacks{ 0 };

			/*
			*	Adds a window closed callback.
			*/
			FORCE_INLINE void AddWindowClosedCallback(const WindowClosedCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_WindowClosedCallbacks.Emplace(callback);
				_WindowClosedCallbackUserData.Emplace(user_data);
				++_NumberOfWindowClosedCallbacks;
			}

			/*
			*	Calls the window closed callbacks.
			*/
			FORCE_INLINE void CallWindowClosedCallbacks() NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfWindowClosedCallbacks; ++i)
				{
					_WindowClosedCallbacks[i](_WindowClosedCallbackUserData[i]);
				}
			}

			//The keyboard button down callbacks.
			DynamicArray<KeyboardButtonDownCallback> _KeyboardButtonDownCallbacks;

			//The keyboard button down callback user data.
			DynamicArray<void *RESTRICT> _KeyboardButtonDownCallbackUserData;

			//The number of keyboard button down callbacks.
			uint64 _NumberOfKeyboardButtonDownCallbacks{ 0 };

			/*
			*	Adds a keyboard button down callback.
			*/
			FORCE_INLINE void AddKeyboardButtonDownCallback(const KeyboardButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_KeyboardButtonDownCallbacks.Emplace(callback);
				_KeyboardButtonDownCallbackUserData.Emplace(user_data);
				++_NumberOfKeyboardButtonDownCallbacks;
			}

			/*
			*	Calls the keyboard button down callbacks.
			*/
			FORCE_INLINE void CallKeyboardButtonDownCallbacks(const KeyboardButton keyboard_button) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfKeyboardButtonDownCallbacks; ++i)
				{
					_KeyboardButtonDownCallbacks[i](keyboard_button, _KeyboardButtonDownCallbackUserData[i]);
				}
			}

			//The keyboard button up callbacks.
			DynamicArray<KeyboardButtonUpCallback> _KeyboardButtonUpCallbacks;

			//The keyboard button up callback user data.
			DynamicArray<void *RESTRICT> _KeyboardButtonUpCallbackUserData;

			//The number of keyboard button up callbacks.
			uint64 _NumberOfKeyboardButtonUpCallbacks{ 0 };

			/*
			*	Adds a keyboard button up callback.
			*/
			FORCE_INLINE void AddKeyboardButtonUpCallback(const KeyboardButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_KeyboardButtonUpCallbacks.Emplace(callback);
				_KeyboardButtonUpCallbackUserData.Emplace(user_data);
				++_NumberOfKeyboardButtonUpCallbacks;
			}

			/*
			*	Calls the keyboard button up callbacks.
			*/
			FORCE_INLINE void CallKeyboardButtonUpCallbacks(const KeyboardButton keyboard_button) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfKeyboardButtonUpCallbacks; ++i)
				{
					_KeyboardButtonUpCallbacks[i](keyboard_button, _KeyboardButtonUpCallbackUserData[i]);
				}
			}

			//The mouse move callbacks.
			DynamicArray<MouseMoveCallback> _MouseMoveCallbacks;

			//The mouse move callback user data.
			DynamicArray<void *RESTRICT> _MouseMoveCallbackUserData;

			//The number of mouse move callbacks.
			uint64 _NumberOfMouseMoveCallbacks{ 0 };

			/*
			*	Adds a mouse move callback.
			*/
			FORCE_INLINE void AddMouseMoveCallback(const MouseMoveCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_MouseMoveCallbacks.Emplace(callback);
				_MouseMoveCallbackUserData.Emplace(user_data);
				++_NumberOfMouseMoveCallbacks;
			}

			/*
			*	Calls the mouse move callbacks.
			*/
			FORCE_INLINE void CallMouseMoveCallbacks(const Vector2<float32> position) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfMouseMoveCallbacks; ++i)
				{
					_MouseMoveCallbacks[i](position, _MouseMoveCallbackUserData[i]);
				}
			}

			//The mouse button down callbacks.
			DynamicArray<MouseButtonDownCallback> _MouseButtonDownCallbacks;

			//The mouse button down callback user data.
			DynamicArray<void *RESTRICT> _MouseButtonDownCallbackUserData;

			//The number of mouse button down callbacks.
			uint64 _NumberOfMouseButtonDownCallbacks{ 0 };

			/*
			*	Adds a mouse button down callback.
			*/
			FORCE_INLINE void AddMouseButtonDownCallback(const MouseButtonDownCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_MouseButtonDownCallbacks.Emplace(callback);
				_MouseButtonDownCallbackUserData.Emplace(user_data);
				++_NumberOfMouseButtonDownCallbacks;
			}

			/*
			*	Calls the mouse button down callbacks.
			*/
			FORCE_INLINE void CallMouseButtonDownCallbacks(const MouseButton mouse_button) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfMouseButtonDownCallbacks; ++i)
				{
					_MouseButtonDownCallbacks[i](mouse_button, _MouseButtonDownCallbackUserData[i]);
				}
			}

			//The mouse button up callbacks.
			DynamicArray<MouseButtonUpCallback> _MouseButtonUpCallbacks;

			//The mouse button up callback user data.
			DynamicArray<void *RESTRICT> _MouseButtonUpCallbackUserData;

			//The number of mouse button up callbacks.
			uint64 _NumberOfMouseButtonUpCallbacks{ 0 };

			/*
			*	Adds a mouse button up callback.
			*/
			FORCE_INLINE void AddMouseButtonUpCallback(const MouseButtonUpCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_MouseButtonUpCallbacks.Emplace(callback);
				_MouseButtonUpCallbackUserData.Emplace(user_data);
				++_NumberOfMouseButtonUpCallbacks;
			}

			/*
			*	Calls the mouse button up callbacks.
			*/
			FORCE_INLINE void CallMouseButtonUpCallbacks(const MouseButton mouse_button) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfMouseButtonUpCallbacks; ++i)
				{
					_MouseButtonUpCallbacks[i](mouse_button, _MouseButtonUpCallbackUserData[i]);
				}
			}

			//The mouse scroll wheel callbacks.
			DynamicArray<MouseScrollWheelCallback> _MouseScrollWheelCallbacks;

			//The mouse scroll wheel callback user data.
			DynamicArray<void *RESTRICT> _MouseScrollWheelCallbackUserData;

			//The number of mouse scroll wheel callbacks.
			uint64 _NumberOfMouseScrollWheelCallbacks{ 0 };

			/*
			*	Adds a mouse scroll wheel callback.
			*/
			FORCE_INLINE void AddMouseScrollWheelCallback(const MouseScrollWheelCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_MouseScrollWheelCallbacks.Emplace(callback);
				_MouseScrollWheelCallbackUserData.Emplace(user_data);
				++_NumberOfMouseScrollWheelCallbacks;
			}

			/*
			*	Calls the mouse scroll wheel callbacks.
			*/
			FORCE_INLINE void CallMouseScrollWheelCallbacks(const int8 step) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _NumberOfMouseScrollWheelCallbacks; ++i)
				{
					_MouseScrollWheelCallbacks[i](step, _MouseScrollWheelCallbackUserData[i]);
				}
			}

			/*
			*	Timer callback data class definition.
			*/
			class TimerCallbackData final
			{

			public:

				//The identifier.
				uint64 _Identifier;

				//The callback.
				TimerCallback _Callback;

				//The user data.
				void *RESTRICT _UserData;

			};

			//The timer callback data.
			DynamicArray<TimerCallbackData> _TimerCallbackData;

			/*
			*	Adds a timer callback.
			*/
			FORCE_INLINE void AddTimerCallback(const uint64 identifier, const TimerCallback callback, void *const RESTRICT user_data) NOEXCEPT
			{
				_TimerCallbackData.Emplace(identifier, callback, user_data);
			}

			/*
			*	Calls the timer callback with the given identifier.
			*/
			FORCE_INLINE void CallTimerCallback(const uint64 identifier) NOEXCEPT
			{
				for (const TimerCallbackData &data : _TimerCallbackData)
				{
					if (data._Identifier == identifier)
					{
						data._Callback(data._UserData);

						return;
					}
				}
			}

			/*
			*	Removes the timer callback with the given identifier.
			*/
			FORCE_INLINE void RemoveTimerCallback(const uint64 identifier) NOEXCEPT
			{
				for (uint64 i{ 0 }; i < _TimerCallbackData.Size(); ++i)
				{
					if (_TimerCallbackData[i]._Identifier == identifier)
					{
						_TimerCallbackData.EraseAt<false>(i);

						return;
					}
				}
			}

		};

	};

}