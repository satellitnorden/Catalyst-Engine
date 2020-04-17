#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

/*
*	File namespace, containing common functions relating to files.
*/
namespace File
{

	/*
	*	Browses for a folder.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE NO_DISCARD bool BrowseForFolder(DynamicString *const RESTRICT chosen_folder) NOEXCEPT;

	/*
	*	Creates a file.
	*/
	FORCE_INLINE void Create(const char *const RESTRICT file) NOEXCEPT;

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE NO_DISCARD bool Exists(const char *const RESTRICT file) NOEXCEPT;

	/*
	*	Deletes a file.
	*/
	FORCE_INLINE void Delete(const char *const RESTRICT file) NOEXCEPT;

}

//Implementation.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsFileCore.inl>
#endif