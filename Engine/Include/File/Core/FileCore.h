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
		CR,
		CRC,
		FBX,
		JPG,
		MID,
		MTL,
		OBJ,
		OGG,
		OPUS,
		PNG,
		TGA,
		TXT,
		WAV,

		UNKNOWN
	};

	/*
	*	Browses for a file.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFile(const bool save, DynamicString* const RESTRICT chosen_file) NOEXCEPT;

	/*
	*	Browses for a folder.
	*	Returns if the action was successful.
	*/
	FORCE_INLINE static NO_DISCARD bool BrowseForFolder(DynamicString* const RESTRICT chosen_folder) NOEXCEPT;

	/*
	*	Creates a directory.
	*/
	FORCE_INLINE static void CreateDirectory(const char *const RESTRICT path) NOEXCEPT;

	/*
	*	Creates a file.
	*/
	FORCE_INLINE static void CreateFile(const char *const RESTRICT path) NOEXCEPT;

	/*
	*	Returns if a file exists or not.
	*/
	FORCE_INLINE static NO_DISCARD bool Exists(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Deletes a file.
	*/
	FORCE_INLINE static void Delete(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Returns the file extension for the given file path.
	*/
	FORCE_INLINE static NO_DISCARD Extension GetExtension(const char* const RESTRICT file_path) NOEXCEPT
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

		if (strcmp(dot_position, ".cr") == 0)
		{
			return Extension::CR;
		}

		if (strcmp(dot_position, ".crc") == 0)
		{
			return Extension::CRC;
		}

		if (strcmp(dot_position, ".fbx") == 0)
		{
			return Extension::FBX;
		}

		if (strcmp(dot_position, ".jpg") == 0)
		{
			return Extension::JPG;
		}

		if (strcmp(dot_position, ".mid") == 0)
		{
			return Extension::MID;
		}

		if (strcmp(dot_position, ".mtl") == 0)
		{
			return Extension::MTL;
		}

		if (strcmp(dot_position, ".obj") == 0)
		{
			return Extension::OBJ;
		}

		if (strcmp(dot_position, ".ogg") == 0)
		{
			return Extension::OGG;
		}

		if (strcmp(dot_position, ".opus") == 0)
		{
			return Extension::OPUS;
		}

		if (strcmp(dot_position, ".png") == 0)
		{
			return Extension::PNG;
		}

		if (strcmp(dot_position, ".tga") == 0)
		{
			return Extension::TGA;
		}

		if (strcmp(dot_position, ".txt") == 0)
		{
			return Extension::TXT;
		}

		if (strcmp(dot_position, ".wav") == 0)
		{
			return Extension::WAV;
		}

		return Extension::UNKNOWN;
	}

	/*
	*	Returns the size of the file with the given file path.
	*/
	FORCE_INLINE NO_DISCARD static uint64 GetSize(const char* const RESTRICT file_path) NOEXCEPT;

}

//Implementation.
#if defined(CATALYST_PLATFORM_ANDROID)
	#include <Platform/Android/AndroidFileCore.inl>
#endif

#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
	#include <Platform/OculusQuest/OculusQuestFileCore.inl>
#endif

#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsFileCore.inl>
#endif