#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

/*
*	File namespace, containing common code relating to files.
*/
namespace File
{

	//Enumeration covering all extensions.
	enum class Extension : uint8
	{
		PNG,
		TGA,
		WAV,

		UNKNOWN
	};

	/*
	*	Browses for a folder.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE NO_DISCARD bool BrowseForFolder(DynamicString* const RESTRICT chosen_folder) NOEXCEPT;

	/*
	*	Creates a file.
	*/
	FORCE_INLINE void Create(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE NO_DISCARD bool Exists(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Deletes a file.
	*/
	FORCE_INLINE void Delete(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Returns the file extension for the given file path.
	*/
	FORCE_INLINE NO_DISCARD Extension GetExtension(const char* const RESTRICT file_path) NOEXCEPT
	{
		//Search for the position of the . character.
		const uint64 string_length{ strlen(file_path) };
		const char* RESTRICT dot_position{ nullptr };

		for (int64 i{ static_cast<int64>(string_length) - 1 }; i >= 0; --i)
		{
			if (file_path[i] == '.')
			{
				dot_position = file_path + i;

				break;
			}
		}

		if (!dot_position)
		{
			return Extension::UNKNOWN;
		}

		if (strcmp(dot_position, ".png") == 0)
		{
			return Extension::PNG;
		}

		if (strcmp(dot_position, ".tga") == 0)
		{
			return Extension::TGA;
		}

		if (strcmp(dot_position, ".wav") == 0)
		{
			return Extension::WAV;
		}

		return Extension::UNKNOWN;
	}

}

//Implementation.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsFileCore.inl>
#endif