#pragma once

/*
*	Platform-specific files.
*/
#if defined(CATALYST_WINDOWS)
#include <Platform/Windows/CatalystWindows.h>
#elif defined(CATALYST_ANDROID)
#include <Platform/Android/CatalystAndroid.h>
#endif