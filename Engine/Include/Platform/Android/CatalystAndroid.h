#if defined(CATALYST_ANDROID)

#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Android specific third party includes.
#include <ThirdParty/android_native_app_glue.h>

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
	*	Pre-updates the platform.
	*/
	static void PreUpdate() NOEXCEPT;

	/*
	*	Post-updates the platform.
	*/
	static void PostUpdate() NOEXCEPT;

	/*
	*	Releases the platform.
	*/
	static void Release() NOEXCEPT;

#if !defined(CATALYST_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif