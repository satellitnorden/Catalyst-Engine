#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//STD.
#include <cstdio>

//Utility macros for printing to a buffer.
#define SPRINTF(BUFFER, FORMAT, ...)													    	\
do																								\
{																								\
	const int32 _CHARACTERS_WRITTEN{ snprintf(BUFFER, sizeof(BUFFER), FORMAT, __VA_ARGS__) };	\
	ASSERT(_CHARACTERS_WRITTEN < sizeof(BUFFER), "Provided too small buffer!");					\
} while (false)

#define SNPRINTF(BUFFER, SIZE, FORMAT, ...)											\
do																					\
{																					\
	const int32 _CHARACTERS_WRITTEN{ snprintf(BUFFER, SIZE, FORMAT, __VA_ARGS__) };	\
	ASSERT(_CHARACTERS_WRITTEN < SIZE, "Provided too small buffer!");				\
} while (false)