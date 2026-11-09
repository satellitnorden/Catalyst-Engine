#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//STD.
#include <cstdio>

//Utility macros for printing to a buffer.
#define SPRINTF(BUFFER, FORMAT, ...)													    	\
do																								\
{																								\
	const int32 characters_written{ snprintf(BUFFER, sizeof(BUFFER), FORMAT, __VA_ARGS__) };	\
	(void)characters_written;																	\
} while (false)

#define SNPRINTF(BUFFER, SIZE, FORMAT, ...)											\
do																					\
{																					\
	const int32 characters_written{ snprintf(BUFFER, SIZE, FORMAT, __VA_ARGS__) };	\
	(void)characters_written;														\
} while (false)