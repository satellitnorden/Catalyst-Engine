#pragma once

/*
*	Platform-specific general files.
*/
#if defined(CATALYST_WINDOWS)
#include <Platform/General/Windows.h>
#elif defined(CATALYST_ANDROID)
#include <Platform/General/Android.h>
#endif