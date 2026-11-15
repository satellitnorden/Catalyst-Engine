#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Third party.
#include <ThirdParty/VrApi/VrAPi.h>

//Android specific third party includes.
#include <android_native_app_glue.h>

/*
*	Main function.
*/
#define MAIN_FUNCTION(implementation)																		\
void android_main(android_app *app)																			\
{																											\
	CatalystPlatform::_App = app;																			\
																											\
	implementation;																							\
}																											\

class CatalystPlatform final
{

public:

	//Handle to the app.
	static android_app *RESTRICT _App;

	//The ovrJava object.
	static ovrJava _ovrJava;

	//The ovrMobile object.
	static ovrMobile *RESTRICT _ovrMobile;

	//Denotes whether or not the app is resumed.
	static bool _IsResumed;

	//Handle to the native window.
	static ANativeWindow *RESTRICT _NativeWindow;

	/*
	*	Initializes the platform.
	*/
	static void Initialize() NOEXCEPT;

	/*
	*	Updates the platform before everything else.
	*/
	static void PlatformPreUpdate() NOEXCEPT;

	/*
	*	Terminates the platform.
	*/
	static void Terminate() NOEXCEPT;

	/*
	*	Returns whether or not the window is in focus.
	*/
	static bool IsWindowInFocus() NOEXCEPT;

	/*
	*	Returns the default resolution.
	*/
	static Resolution GetDefaultResolution() NOEXCEPT;

	/*
	*	Hides the cursor.
	*/
	static void HideCursor() NOEXCEPT;

	/*
	*	Shows the cursor.
	*/
	static void ShowCursor() NOEXCEPT;

	/*
	*	Returns whether or not the cursor is shown.
	*/
	static bool IsCursorShown() NOEXCEPT;

	/*
	*	Returns the scroll wheel step and resets it.
	*/
	static int8 GetAndResetScrollWheelStep() NOEXCEPT;

#if defined(CATALYST_CONFIGURATION_DEBUG)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif