#pragma once

//Core
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class DynamicString;
class KeyboardState;

class CatalystPlatform final
{

public:

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
	static void GetDefaultResolution(uint32 *const RESTRICT width, uint32 *const RESTRICT height) NOEXCEPT;

	/*
	*	Returns the default refresh rate.
	*/
	static float32 GetDefaultRefreshRate() NOEXCEPT;

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

	/*
	*	Retrieves the user name.
	*/
	static bool RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT;

	/*
	*	Shows a banner ad.
	*/
	static void ShowBannerAd() NOEXCEPT;

	/*
	*	Shows an interstitial ad.
	*/
	static void ShowInterstitialAd() NOEXCEPT;

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Prints a string to output.
	*/
	static void PrintToOutput(const char *const RESTRICT format, ...) NOEXCEPT;
#endif

};