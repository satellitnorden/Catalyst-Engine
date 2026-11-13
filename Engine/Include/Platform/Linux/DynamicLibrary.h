#if defined(CATALYST_PLATFORM_LINUX)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	Simple wrapper class for loading a dynamic library (.so) file.
*	TODO: Implement this!
*/
class DynamicLibrary final
{

public:

	/*
	*	Returns if this dynamic library is loaded.
	*/
	FORCE_INLINE NO_DISCARD bool IsLoaded() const NOEXCEPT
	{
		return false;
	}

	/*
	*	Loads this dynamic library.
	*/
	FORCE_INLINE NO_DISCARD bool Load(const char *const RESTRICT file_path) NOEXCEPT
	{
		return false;
	}

	/*
	*	Returns a pointer to the given symbol.
	*/
	FORCE_INLINE NO_DISCARD void *const RESTRICT GetSymbol(const char *const RESTRICT name) NOEXCEPT
	{
		return nullptr;
	}

	/*
	*	Unloads this dynamic library.
	*/
	FORCE_INLINE void Unload() NOEXCEPT
	{
		
	}

};
#endif