#pragma once

/*
*	Platform-specific general files.
*/
#if defined(CATALYST_WINDOWS)
#include <Platform/Windows/Windows.h>
#elif defined(CATALYST_ANDROID)
#include <Platform/Android/Android.h>
#endif