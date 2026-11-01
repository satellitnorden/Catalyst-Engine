#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Constants.
#define DO_VST3_DEBUG_LOGGING (0)

#if DO_VST3_DEBUG_LOGGING
//STD.
#include <filesystem>
#include <fstream>

//Constants.
#define VST3_DEBUG_LOG(MESSAGE) VST3DebugLog(MESSAGE);

FORCE_INLINE static void VST3DebugLog(const char *const RESTRICT message)
{
	constexpr const char *FILE_PATH{ "C:\\VST3 Debug Log.txt" };

	if (!std::filesystem::exists(FILE_PATH))
	{
		std::ofstream file{ FILE_PATH };

		file << "Start!" << std::endl;

		file.close();
	}

	std::ofstream file{ FILE_PATH, std::ofstream::app };

	file << message << std::endl;

	file.close();
}
#else
#define VST3_DEBUG_LOG(MESSAGE)
#endif