#pragma once

/*
*	Platform-specific files.
*/
#if defined(CATALYST_PLATFORM_WINDOWS)
#include <Platform/Windows/CatalystWindows.h>
#elif defined(CATALYST_PLATFORM_ANDROID)
#include <Platform/Android/CatalystAndroid.h>
#endif