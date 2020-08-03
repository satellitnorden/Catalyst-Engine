#pragma once

/*
*	Platform-specific files.
*/
#if defined(CATALYST_PLATFORM_WINDOWS)
#include <Platform/Windows/CatalystPlatformWindows.h>
#elif defined(CATALYST_PLATFORM_ANDROID)
#include <Platform/Android/CatalystPlatformAndroid.h>
#else
#include <Platform/Null/CatalystPlatformNull.h>
#endif