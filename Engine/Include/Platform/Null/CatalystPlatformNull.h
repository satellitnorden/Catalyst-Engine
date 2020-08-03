#if !defined(CATALYST_PLATFORM_ANDROID) && !defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Main function.
*/
#define MAIN_FUNCTION(IMPLEMENTATION)

class CatalystPlatform final
{

public:

	/*
	*	Initializes the platform.
	*/
	static void Initialize() NOEXCEPT
	{

	}

	/*
	*	Returns whether or not the window is in focus.
	*/
	static bool IsWindowInFocus() NOEXCEPT
	{
		return false;
	}

	/*
	*	Hides the cursor.
	*/
	static void HideCursor() NOEXCEPT
	{

	}

	/*
	*	Shows the cursor.
	*/
	static void ShowCursor() NOEXCEPT
	{

	}

	/*
	*	Returns whether or not the cursor is shown.
	*/
	static bool IsCursorShown() NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns the scroll wheel step and resets it.
	*/
	static int8 GetAndResetScrollWheelStep() NOEXCEPT
	{
		return 0;
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT message) NOEXCEPT
	{

	}
#endif

};

#endif