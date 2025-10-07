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
		const uint64 string_length{ StringUtilities::StringLength(file_path) };
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

		if (StringUtilities::IsEqual(dot_position, ".ca"))
		{
			return Extension::CA;
		}

		if (StringUtilities::IsEqual(dot_position, ".cac"))
		{
			return Extension::CAC;
		}

		if (StringUtilities::IsEqual(dot_position, ".cr"))
		{
			return Extension::CR;
		}

		if (StringUtilities::IsEqual(dot_position, ".crc"))
		{
			return Extension::CRC;
		}

		if (StringUtilities::IsEqual(dot_position, ".fbx"))
		{
			return Extension::FBX;
		}

		if (StringUtilities::IsEqual(dot_position, ".jpg"))
		{
			return Extension::JPG;
		}

		if (StringUtilities::IsEqual(dot_position, ".mid"))
		{
			return Extension::MID;
		}

		if (StringUtilities::IsEqual(dot_position, ".mp3"))
		{
			return Extension::MP3;
		}

		if (StringUtilities::IsEqual(dot_position, ".mtl"))
		{
			return Extension::MTL;
		}

		if (StringUtilities::IsEqual(dot_position, ".obj"))
		{
			return Extension::OBJ;
		}

		if (StringUtilities::IsEqual(dot_position, ".ogg"))
		{
			return Extension::OGG;
		}

		if (StringUtilities::IsEqual(dot_position, ".opus"))
		{
			return Extension::OPUS;
		}

		if (StringUtilities::IsEqual(dot_position, ".png"))
		{
			return Extension::PNG;
		}

		if (StringUtilities::IsEqual(dot_position, ".tga"))
		{
			return Extension::TGA;
		}

		if (StringUtilities::IsEqual(dot_position, ".txt"))
		{
			return Extension::TXT;
		}

		if (StringUtilities::IsEqual(dot_position, ".wav"))
		{
			return Extension::WAV;
		}

		if (StringUtilities::IsEqual(dot_position, ".xml"))
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

	/*
	*	Returns the last changed time for the given directory/file path.
	*	Expressed in seconds since epoch.
	*/
	NO_DISCARD uint64 LastChangedTime(const char *const RESTRICT directory_or_file_path) NOEXCEPT;

}