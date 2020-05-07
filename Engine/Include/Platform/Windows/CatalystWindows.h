#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Windows.
#include <Windows.h>
#if !defined(INITGUID)
	#define INITGUID
#endif
#include <ShlObj_core.h>

//These technically don't need to be public through the entire engine, but need to be included before the undef's.
#include <audioclient.h>
#include <mmdeviceapi.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CopyMemory
#undef CreateFile
#undef CreateFont
#undef CreateEvent
#undef CreateSemaphore
#undef GetCurrentTime
#undef INFINITE
#undef MemoryBarrier
#undef far
#undef max
#undef min
#undef near
#undef PlaySound
#undef Yield

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
class MouseState;

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
	*	Returns whether or not the window is in focus.
	*/
	static bool IsWindowInFocus() NOEXCEPT;

	/*
	*	Hides the cursor.
	*/
	static void HideCursor() NOEXCEPT;

	/*
	*	Shows the cursor.
	*/
	static void ShowCursor() NOEXCEPT;

	/*
	*	Returns whether or not the cursor is shown.
	*/
	static bool IsCursorShown() NOEXCEPT;

	/*
	*	Returns the scroll wheel step and resets it.
	*/
	static int8 GetAndResetScrollWheelStep() NOEXCEPT;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif