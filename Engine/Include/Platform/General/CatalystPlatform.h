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
	*	Updates the platform after everything else.
	*/
	static void PlatformPostUpdate() NOEXCEPT;

	/*
	*	Terminates the platform.
	*/
	static void Terminate() NOEXCEPT;

	/*
	*	Creates a platform window with the given parameters.
	*/
	static void *const RESTRICT CreatePlatformWindow(const char *const RESTRICT name, const uint32 width, const uint32 height, const bool fullscreen) NOEXCEPT;

	/*
	*	Destroys a platform window.
	*/
	static void DestroyPlayformWindow(void *const RESTRICT window) NOEXCEPT;

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
	*	Returns the number of input characters for this frame.
	*/
	static NO_DISCARD uint64 NumberOfInputCharacters() NOEXCEPT;

	/*
	*	Returns the input character at the given index for this frame.
	*/
	static NO_DISCARD char InputCharacterAt(const uint64 index) NOEXCEPT;

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