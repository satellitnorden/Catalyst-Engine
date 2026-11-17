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

#include <Platform/Platforms/Windows/WindowsUndefines.h>

class CatalystPlatformWindows final
{

public:

	//Handle to the instance.
	static HINSTANCE _Instance;

	//The input characters.
	static DynamicArray<char> _InputCharacters;

	/*
	*	The crash handler.
	*/
	static LONG WINAPI CrashHandler(EXCEPTION_POINTERS *exception_info) NOEXCEPT;

private:

};
#endif