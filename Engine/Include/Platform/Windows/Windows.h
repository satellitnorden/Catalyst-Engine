#if defined(CATALYST_WINDOWS)

#pragma once

/*
*	Windows specific includes.
*/
#include <Platform/Rendering/API/Vulkan/WindowsPlatformVulkan.h>

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
*	Main function
*/
#define MAIN_FUNCTION(implementation) int32 main(const int32 argumentCount, char *RESTRICT arguments[]) NOEXCEPT	\
{																													\
	implementation;																									\
																													\
	return 0;																										\
}																													\

#endif
