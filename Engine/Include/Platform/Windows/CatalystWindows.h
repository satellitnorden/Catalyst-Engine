#if defined(CATALYST_WINDOWS)

#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

/*
*	Windows specific third party includes.
*/
#include <Windows.h>
#include <Xinput.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CopyMemory
#undef CreateEvent
#undef CreateSemaphore
#undef max
#undef min
#undef PlaySound

/*
*	Main function.
*/
#define MAIN_FUNCTION(implementation) int32 WINAPI WinMain(	_In_ HINSTANCE hInstance,		\
															_In_ HINSTANCE hPrevInstance,	\
															_In_opt_ LPSTR lpCmdLine,		\
															_In_ int32 nCmdShow)			\
{																							\
	CatalystPlatform::_Instance = hInstance;												\
	CatalystPlatform::_ShowCommand = nCmdShow;												\
																							\
	implementation;																			\
																							\
	return 0;																				\
}																							\

//Forward declarations.
class GamepadState;
class KeyboardState;
class MouseState;
class TouchState;

class CatalystPlatform final
{

public:

	//Handle to the instance.
	static HINSTANCE _Instance;

	//The command to show the window.
	static int32 _ShowCommand;

	//Handle to the window.
	static HWND _Window;

	/*
	*	Initializes the platform.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Post-nitializes the platform.
	*/
	static void PostInitialize() NOEXCEPT;

	/*
	*	Pre-updates the platform.
	*/
	static void PreUpdate() NOEXCEPT;

	/*
	*	Post-updates the platform.
	*/
	static void PostUpdate() NOEXCEPT;

	/*
	*	Releases the platform.
	*/
	static void Release() NOEXCEPT;

	/*
	*	Retrieves the current gamepad state.
	*/
	static void GetCurrentGamepadState(const uint8 index, GamepadState *const RESTRICT state) NOEXCEPT;

	/*
	*	Retrieves the current keyboard state.
	*/
	static void GetCurrentKeyboardState(KeyboardState *const RESTRICT state) NOEXCEPT;

	/*
	*	Retrieves the current mouse state.
	*/
	static void GetCurrentMouseState(MouseState *const RESTRICT state) NOEXCEPT;

	/*
	*	Retrieves the current touch state.
	*/
	static void GetCurrentTouchState(TouchState *const RESTRICT state) NOEXCEPT;

#if !defined(CATALYST_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif