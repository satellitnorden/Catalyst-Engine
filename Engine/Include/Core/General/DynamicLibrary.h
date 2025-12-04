#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Windows/CatalystPlatformWindows.h>

/*
*	Simple wrapper class for loading a dynamic library (.dll) file.
*/
class DynamicLibrary final
{

public:

	/*
	*	Loads this dynamic library.
	*/
	FORCE_INLINE NO_DISCARD bool Load(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Unload the previous handle, if there is one.
		if (_Handle)
		{
			Unload();
		}

		//Load the library.
		_Handle = LoadLibraryA(file_path);

		//Return if the load succeeded.
		return _Handle != nullptr;
	}

	/*
	*	Returns a pointer to the given symbol.
	*/
	FORCE_INLINE NO_DISCARD void *const RESTRICT GetSymbol(const char *const RESTRICT name) NOEXCEPT
	{
		return static_cast<void *const RESTRICT>(GetProcAddress(_Handle, name));
	}

	/*
	*	Unloads this dynamic library.
	*/
	FORCE_INLINE void Unload() NOEXCEPT
	{
		if (!_Handle)
		{
			return;
		}

		//Free the library.
		FreeLibrary(_Handle);

		//Reset the handle.
		_Handle = nullptr;
	}

private:

	//The handle.
	HMODULE _Handle{ nullptr };

};