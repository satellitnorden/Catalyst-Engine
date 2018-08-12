#if defined(CATALYST_ANDROID)

#pragma once

//Android specific third party includes.
#include <ThirdParty/android_native_app_glue.h>

//Android specific includes.
#include <Platform/Android/AndroidPlatformVulkan.h>

/*
*	Main function.
*/
#define MAIN_FUNCTION(implementation)																		\
void android_main(android_app *app)																			\
{																											\
	CatalystPlatform::app = app;																			\
																											\
	implementation;																							\
}																											\

class CatalystPlatform final
{

public:

	//Handle to the app.
	static android_app *RESTRICT app;

	//Handle to the window.
	static ANativeWindow *RESTRICT window;

	/*
	*	Initializes the platform.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Post-nitializes the platform.
	*/
	static void PostInitialize() NOEXCEPT;

	/*
	*	Updates the platform.
	*/
	static void Update() NOEXCEPT;

	/*
	*	Releases the platform.
	*/
	static void Release() NOEXCEPT;

};

#endif