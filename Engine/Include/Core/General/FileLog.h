#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Print.h>

//STD.
#include <fstream>

//Appends the given contents to the given file.
#define LOG_FILE_APPEND(FILE_PATH, ...)             \
{                                                   \
    std::ofstream file{ FILE_PATH, std::ios::app }; \
    file << __VA_ARGS__ << std::endl;               \
    file.close();                                   \
}