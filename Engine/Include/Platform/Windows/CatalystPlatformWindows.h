#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//ASIO library. Needs to be included before everything else. (:
#if defined(_WIN32)
	#define _WIN32_WINNT 0x0A00
#endif

//Windows.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if !defined(INITGUID)
	#define INITGUID
#endif
#include <ShlObj_core.h>
#include <tchar.h>

//These technically don't need to be public through the entire engine, but need to be included before the undef's.
#include <audioclient.h>
#include <mmdeviceapi.h>

#include <Platform/Windows/WindowsUndefines.h>

class CatalystPlatformWindows final
{

public:

	//Handle to the instance.
	static HINSTANCE _Instance;

	//The command to show the window.
	static int32 _ShowCommand;

	//Handle to the window.
	static HWND _Window;

	//The key down events.
	static DynamicArray<uint64> _KeyDownEvents;

	//The key up events.
	static DynamicArray<uint64> _KeyUpEvents;

	//The input characters.
	static DynamicArray<char> _InputCharacters;

};

/*
*	Main function.
*/
#define MAIN_FUNCTION(IMPLEMENTATION) int32 WINAPI WinMain(	_In_ HINSTANCE hInstance,		\
															_In_ HINSTANCE hPrevInstance,	\
															_In_opt_ LPSTR lpCmdLine,		\
															_In_ int32 nCmdShow)			\
{																							\
	CatalystPlatformWindows::_Instance = hInstance;											\
	CatalystPlatformWindows::_ShowCommand = nCmdShow;										\
																							\
	IMPLEMENTATION;																			\
																							\
	return 0;																				\
}
#endif