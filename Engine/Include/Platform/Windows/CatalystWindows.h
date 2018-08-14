#if defined(CATALYST_WINDOWS)

#pragma once

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
*	Windows specific includes.
*/
#include <Platform/Windows/WindowsPlatformVulkan.h>

/*
*	Main function.
*/
#define MAIN_FUNCTION(implementation) int32 CALLBACK WinMain(	_In_ HINSTANCE hInstance,		\
																_In_ HINSTANCE hPrevInstance,	\
																_In_ LPSTR lpCmdLine,			\
																_In_ int32 nCmdShow) NOEXCEPT	\
{																								\
	CatalystPlatform::instance = hInstance;														\
	CatalystPlatform::showCommand = nCmdShow;													\
																								\
	implementation;																				\
																								\
	return 0;																					\
}																								\

class CatalystPlatform final
{

public:

	//Handle to the instance.
	static HINSTANCE instance;

	//The command to show the window.
	static int32 showCommand;

	//Handle to the window.
	static HWND window;

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

};

#endif
