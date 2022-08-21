#if defined(CATALYST_PLATFORM_ANDROID)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Input.
#include <Input/InputCore.h>

//Rendering.
#include <Rendering/Native/Resolution.h>

//Android specific third party includes.
#include <android_native_app_glue.h>

/*
*	Android touch state class definition.
*/
class AndroidTouchState final
{

public:

	//The current X.
	float32 _CurrentX{ 0.0f };

	//The current Y.
	float32 _CurrentY{ 0.0f };

	//Is the screen currently touched?
	bool _IsCurrentlyTouched{ false };

};

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

	//Handle to the window.
	static ANativeWindow *RESTRICT _Window;

	//The current Android touch states.
	static StaticArray<AndroidTouchState, InputConstants::MAXIMUM_NUMBER_OF_TOUCH_FINGERS> _CurrentAndroidTouchStates;

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