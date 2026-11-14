#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Platform/Core/Main.h>

//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

/*
*	The main function.
*/
int32 WINAPI WinMain
(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_opt_ LPSTR lpCmdLine,
	_In_ int32 nCmdShow)
{
	//Set up the crash handler.
	SetUnhandledExceptionFilter(CatalystPlatformWindows::CrashHandler);

	//Set the instance/show command.
	CatalystPlatformWindows::_Instance = hInstance;
	CatalystPlatformWindows::_ShowCommand = nCmdShow;

	//Call the game's main function.
	Main();

	//Return success!
	return 0;
}
#endif