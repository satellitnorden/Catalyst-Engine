#pragma once

//Core
#include <Core/Essential/CatalystEssential.h>

//Forward declarations.
class DynamicString;
class KeyboardState;
class MouseState;

class CatalystPlatform final
{

public:

	/*
	*	Window description class definition.
	*/
	class WindowDescription final
	{

	public:

		/*
		*	Enumeration covering all flags.
		*/
		enum class Flag : uint8
		{
			/*
			*	No flags. (:
			*/
			NONE = 0,

			/*
			*	Denotes that this is a fullscreen window.
			*/
			FULLSCREEN = BIT(0),

			/*
			*	Denotes that this is a windowed window.
			*/
			WINDOWED = BIT(1),

			/*
			*	Denotes that this is a popup window.
			*/
			POPUP = BIT(2),

			/*
			*	Denotes that this is a child window.
			*/
			CHILD = BIT(3)
		};

		//The title.
		char _Title[64];

		//The width.
		uint32 _Width;

		//The height.
		uint32 _Height;

		//The flags.
		Flag _Flags;

	};

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
	*	Returns the directory path of the current executable.
	*/
	static NO_DISCARD const char *const RESTRICT GetExecutableDirectorypath() NOEXCEPT;

	/*
	*	Returns if the given platform window is alive.
	*/
	static NO_DISCARD bool IsPlatformWindowAlive(void *const RESTRICT window) NOEXCEPT;

	/*
	*	Creates a platform window with the given description.
	*/
	static void *const RESTRICT CreatePlatformWindow(const WindowDescription &description) NOEXCEPT;

	/*
	*	Sets the size of a platform window.
	*/
	static void SetPlatformWindowSize(void *const RESTRICT window, const uint32 width, const uint32 height) NOEXCEPT;

	/*
	*	Shows the given platform window.
	*/
	static void ShowPlatformWindow(void *const RESTRICT window) NOEXCEPT;

	/*
	*	Hides the given platform window.
	*/
	static void HidePlatformWindow(void *const RESTRICT window) NOEXCEPT;

	/*
	*	Destroys a platform window.
	*/
	static void DestroyPlayformWindow(void *const RESTRICT window) NOEXCEPT;

	/*
	*	Returns whether or not the window is in focus.
	*/
	static bool IsWindowInFocus(void *const RESTRICT window) NOEXCEPT;

	/*
	*	Returns the default resolution.
	*/
	static void GetDefaultResolution(uint32 *const RESTRICT width, uint32 *const RESTRICT height) NOEXCEPT;

	/*
	*	Returns the default refresh rate.
	*/
	static float32 GetDefaultRefreshRate() NOEXCEPT;

	/*
	*	Updates the given mouse state for the given window.
	*	If window is nullptr, then the main window will be used.
	*/
	static void UpdateMouseState(MouseState *const RESTRICT mouse_state, void *const RESTRICT window) NOEXCEPT;

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

ENUMERATION_BIT_OPERATIONS(CatalystPlatform::WindowDescription::Flag);