#pragma once

/*
*	Platform-specific files.
*/
#if defined(CATALYST_PLATFORM_ANDROID)
	#include <Platform/Android/CatalystPlatformAndroid.h>
#elif defined(CATALYST_PLATFORM_OCULUS_QUEST)
	#include <Platform/OculusQuest/CatalystPlatformOculusQuest.h>
#elif defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/CatalystPlatformWindows.h>
#else
	#include <Platform/Null/CatalystPlatformNull.h>
#endif