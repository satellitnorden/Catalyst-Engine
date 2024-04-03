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
		CA,
		CAC,
		CR,
		CRC,
		FBX,
		JPG,
		MID,
		MP3,
		MTL,
		OBJ,
		OGG,
		OPUS,
		PNG,
		TGA,
		TXT,
		WAV,
		XML,

		UNKNOWN
	};

	/*
	*	Returns the file extension for the given file path.
	*/
	FORCE_INLINE NO_DISCARD Extension GetExtension(const char *const RESTRICT file_path) NOEXCEPT
	{
		//Search for the position of the . character.
		const uint64 string_length{ strlen(file_path) };
		const char *RESTRICT dot_position{ nullptr };

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

		if (strcmp(dot_position, ".ca") == 0)
		{
			return Extension::CA;
		}

		if (strcmp(dot_position, ".cac") == 0)
		{
			return Extension::CAC;
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

		if (strcmp(dot_position, ".mp3") == 0)
		{
			return Extension::MP3;
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

		if (strcmp(dot_position, ".xml") == 0)
		{
			return Extension::XML;
		}

		return Extension::UNKNOWN;
	}

	/*
	*	Browses for a file.
	*	Returns if the action was successful.
	*/
	NO_DISCARD bool BrowseForFile(const bool save, DynamicString* const RESTRICT chosen_file, const char *const RESTRICT filter = nullptr) NOEXCEPT;

	/*
	*	Browses for a folder.
	*	Returns if the action was successful.
	*/
	NO_DISCARD bool BrowseForFolder(DynamicString* const RESTRICT chosen_folder) NOEXCEPT;

	/*
	*	Creates a directory.
	*/
	void CreateDirectory(const char *const RESTRICT path) NOEXCEPT;

	/*
	*	Creates a file.
	*/
	void CreateFile(const char *const RESTRICT path) NOEXCEPT;

	/*
	*	Returns if a file exists or not.
	*/
	NO_DISCARD bool Exists(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Deletes a file.
	*/
	void Delete(const char* const RESTRICT file) NOEXCEPT;

	/*
	*	Returns the size of the file with the given file path.
	*/
	NO_DISCARD uint64 GetSize(const char *const RESTRICT file_path) NOEXCEPT;

}