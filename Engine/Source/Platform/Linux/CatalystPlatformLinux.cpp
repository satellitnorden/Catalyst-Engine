#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
//#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Returns the CPU information.
*/
NO_DISCARD CatalystPlatform::CPUInformation CatalystPlatform::GetCPUInformation() NOEXCEPT
{
	CPUInformation cpu_information;

	//Retrieve if this CPU supports AVX2 instructions.
	cpu_information._SupportsAVX2 = false;

	return cpu_information;
}

/*
*	Initializes the platform.
*/
void CatalystPlatform::Initialize() NOEXCEPT
{
	
}

/*
*	Updates the platform before everything else.
*/
void CatalystPlatform::PlatformPreUpdate() NOEXCEPT
{

}

/*
*	Updates the platform after everything else.
*/
void CatalystPlatform::PlatformPostUpdate() NOEXCEPT
{
	
}

/*
*	Terminates the platform.
*/
void CatalystPlatform::Terminate() NOEXCEPT
{

}

/*
*	Returns the directory path of the current executable.
*/
NO_DISCARD const char *const RESTRICT CatalystPlatform::GetExecutableDirectorypath() NOEXCEPT
{
	return nullptr;
}

/*
*	Sets the size of a platform window.
*/
void CatalystPlatform::SetPlatformWindowSize(void *const RESTRICT window, const uint32 width, const uint32 height) NOEXCEPT
{
	
}

/*
*	Shows the given platform window.
*/
void CatalystPlatform::ShowPlatformWindow(void *const RESTRICT window) NOEXCEPT
{
	
}

/*
*	Hides the given platform window.
*/
void CatalystPlatform::HidePlatformWindow(void *const RESTRICT window) NOEXCEPT
{
	
}

/*
*	Destroys a platform window.
*/
void CatalystPlatform::DestroyPlayformWindow(void *const RESTRICT window) NOEXCEPT
{
	
}

/*
*	Returns whether or not the window is in focus.
*/
bool CatalystPlatform::IsWindowInFocus(void *const RESTRICT window) NOEXCEPT
{
	return false;	
}

/*
*	Returns the default resolution.
*/
void CatalystPlatform::GetDefaultResolution(uint32 *const RESTRICT width, uint32 *const RESTRICT height) NOEXCEPT
{
	*width = 1'920;
	*height = 1'080;
}

/*
*	Returns the default refresh rate.
*/
float32 CatalystPlatform::GetDefaultRefreshRate() NOEXCEPT
{
	return 1.0f / 60.0f;
}

/*
*	Updates the given mouse state for the given window.
*	If window is nullptr, then the main window will be used.
*/
void CatalystPlatform::UpdateMouseState(MouseState *const RESTRICT mouse_state, void *const RESTRICT window) NOEXCEPT
{
	
}

/*
*	Returns the number of input characters for this frame.
*/
NO_DISCARD uint64 CatalystPlatform::NumberOfInputCharacters() NOEXCEPT
{
	return 0;
}

/*
*	Returns the input character at the given index for this frame.
*/
NO_DISCARD char CatalystPlatform::InputCharacterAt(const uint64 index) NOEXCEPT
{
	return 0;
}

/*
*	Hides the cursor.
*/
void CatalystPlatform::HideCursor() NOEXCEPT
{
	
}

/*
*	Shows the cursor.
*/
void CatalystPlatform::ShowCursor() NOEXCEPT
{
	
}

/*
*	Returns whether or not the cursor is shown.
*/
bool CatalystPlatform::IsCursorShown() NOEXCEPT
{
	return false;
}

/*
*	Retrieves the user name.
*/
bool CatalystPlatform::RetrieveUserName(DynamicString *const RESTRICT output) NOEXCEPT
{
	return false;
}

void CatalystPlatform::ShowBannerAd() NOEXCEPT
{
	
}

/*
*	Shows an interstitial ad.
*/
void CatalystPlatform::ShowInterstitialAd() NOEXCEPT
{
	
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints a string to output.
*/
void CatalystPlatform::PrintToOutput(const char *const RESTRICT format, ...) NOEXCEPT
{
	
}
#endif
#endif