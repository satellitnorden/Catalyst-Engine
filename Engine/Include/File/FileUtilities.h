#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace FileUtilities
{

	/*
	*	Creates a file.
	*/
	FORCE_INLINE static void CreateFile(const char *const RESTRICT file) NOEXCEPT
	{
		const HANDLE file_handle{ CreateFileA(	file,
												0,
												0,
												nullptr,
												CREATE_ALWAYS,
												FILE_ATTRIBUTE_NORMAL,
												nullptr) };

		CloseHandle(file_handle);
	}

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE static NO_DISCARD bool DoesFileExist(const char* const RESTRICT file) NOEXCEPT
	{
		return GetFileAttributes(file) != INVALID_FILE_SIZE;
	}

}