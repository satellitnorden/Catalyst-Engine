#pragma once

//Core.
#include <Core/Core.h>

#if defined(PLATFORM_WIN64)
	#include <Platform/PlatformWin64.h>
#endif

#if defined(PLATFORM_LINUX)
	#include <Platform/PlatformLinux.h>
#endif