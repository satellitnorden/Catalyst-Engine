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
	CatalystWindows::instance = hInstance;														\
																								\
	implementation;																				\
																								\
	return 0;																					\
}																								\

/*
*	Main loop.
*/
#define MAIN_LOOP(implementation)					\
while (!EngineSystem::Instance->ShouldTerminate())	\
{													\
	implementation;									\
}													\

/*
*	Catalyst Windows.
*/
class CatalystWindows final
{

public:

	//The instance handle.
	static HINSTANCE instance;

};

#endif
