#if defined(CATALYST_PLATFORM_WINDOWS)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>

namespace Platform
{

	/*
	*	Simple wrapper class for loading a dynamic library (.dll) file.
	*/
	class DynamicLibrary final
	{

	public:

		/*
		*	Returns if this dynamic library is loaded.
		*/
		FORCE_INLINE NO_DISCARD bool IsLoaded() const NOEXCEPT
		{
			return _Handle != nullptr;
		}

		/*
		*	Loads this dynamic library.
		*/
		FORCE_INLINE NO_DISCARD bool Load(const char *const RESTRICT file_path) NOEXCEPT
		{
			//Unload the previous handle, if there is one.
			if (IsLoaded())
			{
				Unload();
			}

			//Load the library.
			_Handle = LoadLibraryA(file_path);

			//Return if the load succeeded.
			return IsLoaded();
		}

		/*
		*	Initializes this dynamic library.
		*/
		FORCE_INLINE NO_DISCARD bool Initialize() NOEXCEPT
		{
			//Type aliases.
			using InitModuleProc = bool(*)();

			//Try to call the 'InitDll' symbol.
			if (InitModuleProc symbol{ reinterpret_cast<InitModuleProc>(GetSymbol("InitDll")) })
			{
				return symbol();
			}

			//Try to call the 'ModuleEntry' symbol.
			if (InitModuleProc symbol{ reinterpret_cast<InitModuleProc>(GetSymbol("ModuleEntry")) })
			{
				return symbol();
			}

			//Otherwise, this dynamic library may not expose an entry function, assume it succeeds.
			return true;
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
			if (!IsLoaded())
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

}
#endif