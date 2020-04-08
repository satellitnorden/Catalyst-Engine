#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	File namespace, containing common functions relating to files.
*/
namespace File
{

	/*
	*	Creates a file.
	*/
	FORCE_INLINE void Create(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE NO_DISCARD bool Exists(const char* const RESTRICT file) NOEXCEPT;

}

//Implementation.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsFileCore.inl>
#endif