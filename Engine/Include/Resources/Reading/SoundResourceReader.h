#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//File handling.
#include <FileHandling/BinaryFile.h>

//Resources.
#include <Resources/Reading/SoundResource.h>

class SoundResourceReader final
{

public:

	/*
	*	Reads the sound resource at the given file path.
	*/
	FORCE_INLINE static void Read(const char* const RESTRICT file_path, SoundResource* const RESTRICT resource) NOEXCEPT
	{
		ASSERT(resource, "SoundResourceReader::Read - Invalid resource passed!");

		//Open the file.
		BinaryFile<IOMode::In> file{ file_path };

		//Close the file.
		file.Close();
	}

};