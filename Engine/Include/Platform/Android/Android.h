#if defined(CATALYST_ANDROID)

#pragma once

/*
*	Android specific includes.
*/
#include <Platform/Android/AndroidPlatformVulkan.h>

/*
*	Android specific third party includes.
*/
#include <ThirdParty/android_native_app_glue.h>

/*
*	Main function
*/
#define MAIN_FUNCTION(implementation) void android_main(struct android_app *state)									\
{																													\
	implementation;																									\
}																													\

#endif