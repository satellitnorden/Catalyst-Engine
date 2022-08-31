#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//ASIO library. Needs to be included before everything else. (:
#if defined(_WIN32)
	#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
__pragma(warning(disable : 4530)) //Disable warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc.
#include <ThirdParty/asio/asio.hpp>
#include <ThirdParty/asio/asio/ts/buffer.hpp>
#include <ThirdParty/asio/asio/ts/internet.hpp>

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Windows.
#include <Windows.h>
#if !defined(INITGUID)
#define INITGUID
#endif
#include <ShlObj_core.h>
#include <tchar.h>

//These technically don't need to be public through the entire engine, but need to be included before the undef's.
#include <audioclient.h>
#include <mmdeviceapi.h>

//Undefine annoying Windows defines. Fuck Windows. I hate everything.
#undef CopyMemory
#undef CreateDirectory
#undef CreateFile
#undef CreateFont
#undef CreateEvent
#undef CreateSemaphore
#undef FAR
#undef GetCurrentTime
#undef INFINITE
#undef MemoryBarrier
#undef NEAR
#undef OPAQUE
#undef far
#undef max
#undef min
#undef near
#undef PlaySound
#undef Yield
#undef IN
#undef OUT
#undef ERROR

/*
*	Main function.
*/
#define MAIN_FUNCTION(IMPLEMENTATION) int32 WINAPI WinMain(	_In_ HINSTANCE hInstance,		\
															_In_ HINSTANCE hPrevInstance,	\
															_In_opt_ LPSTR lpCmdLine,		\
															_In_ int32 nCmdShow)			\
{																							\
	CatalystPlatform::_Instance = hInstance;												\
	CatalystPlatform::_ShowCommand = nCmdShow;												\
																							\
	IMPLEMENTATION;																			\
																							\
	return 0;																				\
}																							\

//Forward declarations.
class DynamicString;
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
	*	Updates the platform before everything else.
	*/
	static void PlatformPreUpdate() NOEXCEPT;

	/*
	*	Terminates the platform.
	*/
	static void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not the window is in focus.
	*/
	static bool IsWindowInFocus() NOEXCEPT;

	/*
	*	Returns the default resolution.
	*/
	static Resolution GetDefaultResolution() NOEXCEPT;

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

	/*
	*	Retrieves the user name.
	*/
	static bool RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT;

	/*
	*	Shows a banner ad.
	*/
	static void ShowBannerAd() NOEXCEPT;

	/*
	*	Shows an interstitial ad.
	*/
	static void ShowInterstitialAd() NOEXCEPT;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif