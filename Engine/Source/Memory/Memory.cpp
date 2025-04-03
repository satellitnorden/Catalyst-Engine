//Core.
#include <Core/Essential/CatalystEssential.h>

//Header file.
#include <Memory/Memory.h>

//STL.
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Prints a memory string into the given buffer.
*/
void Memory::PrintMemoryString(char *buffer, const uint64 buffer_size, const char *const RESTRICT prefix, const uint64 input_bytes) NOEXCEPT
{
	if (input_bytes > 1'000)
	{
		if (input_bytes > 1'000'000)
		{
			const float64 memory{ static_cast<float64>(input_bytes) / 1'000'000.0 };

			sprintf_s(buffer, buffer_size, "%s: %.2f MB", prefix, memory);
		}

		else
		{
			const float64 memory{ static_cast<float64>(input_bytes) / 1'000.0 };

			sprintf_s(buffer, buffer_size, "%s: %.2f KB", prefix, memory);
		}
	}

	else
	{
		const float64 memory{ static_cast<float64>(input_bytes) };

		sprintf_s(buffer, buffer_size, "%s: %.2f B", prefix, memory);
	}
}
#endif

/*
*	Allocates a chunk of memory internally.
*/
RESTRICTED NO_DISCARD void *const RESTRICT Memory::AllocateInternal(const uint64 size) NOEXCEPT
{
	return malloc(size);
}

/*
*	Reallocates a chunk of memory previously allocated with Allocate() internally.
*/
RESTRICTED NO_DISCARD void *const RESTRICT Memory::ReallocateInternal(void *const RESTRICT memory, const uint64 size) NOEXCEPT
{
	return realloc(memory, size);
}

/*
*	Frees internally.
*/
void Memory::FreeInternal(void *const RESTRICT memory) NOEXCEPT
{
	free(memory);
}

/*
*	Copies memory from the destination to the source internally.
*/
void Memory::CopyInternal(void *const RESTRICT destination, const void *const RESTRICT source, const uint64 size) NOEXCEPT
{
	memcpy(destination, source, size);
}

/*
*	Sets a chunk of memory to the specified value internally.
*/
void Memory::SetInternal(void *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
{
	memset(destination, value, size);
}

/*
*	Compares two chunks of memory. Returns true if the chunks are identical, false otherwise internally.
*/
NO_DISCARD bool Memory::CompareInternal(const void *const RESTRICT first, const void *const RESTRICT second, const uint64 size) NOEXCEPT
{
	return memcmp(first, second, size) == 0;
}