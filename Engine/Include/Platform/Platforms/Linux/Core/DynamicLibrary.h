#if defined(CATALYST_PLATFORM_LINUX)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/LogSystem.h>

//Third party.
#include <dlfcn.h>

namespace Platform
{

	/*
	*	Simple wrapper class for loading a dynamic library (.so) file.
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
			//Try to open the file.
			_Handle = dlopen(file_path, RTLD_LAZY);

			if (!_Handle)
			{
				LOG_ERROR("Failed to open '%s' dynamic library: %s", file_path, dlerror());
			}

			return IsLoaded();
		}

		/*
		*	Initializes this dynamic library.
		*/
		FORCE_INLINE NO_DISCARD bool Initialize() NOEXCEPT
		{
			//TODO: Maybe there's some Linux-specific initialize function for dynamic libraries?
			return true;
		}

		/*
		*	Returns a pointer to the given symbol.
		*/
		FORCE_INLINE NO_DISCARD void *const RESTRICT GetSymbol(const char *const RESTRICT name) NOEXCEPT
		{
			return dlsym(_Handle, name);
		}

		/*
		*	Unloads this dynamic library.
		*/
		FORCE_INLINE void Unload() NOEXCEPT
		{
			if (_Handle)
			{
				dlclose(_Handle);
				_Handle = nullptr;
			}
		}

	private:

		//The handle.
		void *RESTRICT _Handle{ nullptr };

	};

}
#endif