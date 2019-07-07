#if defined(CATALYST_PLATFORM_ANDROID)

#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Android specific third party includes.
#include <ThirdParty/android_native_app_glue.h>

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

//Forward declarations.
class GamepadState;
class KeyboardState;
class MouseState;
class TouchState;

class CatalystPlatform final
{

public:

	//Handle to the app.
	static android_app *RESTRICT _App;

	//Handle to the window.
	static ANativeWindow *RESTRICT _Window;

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
	static void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Post-updates the platform.
	*/
	static void PostUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Releases the platform.
	*/
	static void Release() NOEXCEPT;

	/*
	*	Retrieves the current gamepad state.
	*/
	static void GetCurrentGamepadState(const uint8 index, GamepadState *const RESTRICT state) NOEXCEPT;

	/*
	*	Retrieves the current keyboard state.
	*/
	static void GetCurrentKeyboardState(KeyboardState *const RESTRICT state) NOEXCEPT;

	/*
	*	Retrieves the current mouse state.
	*/
	static void GetCurrentMouseState(MouseState *const RESTRICT state) NOEXCEPT;

#if defined(CATALYST_CONFIGURATION_DEBUG)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT;
#endif

};

#endif