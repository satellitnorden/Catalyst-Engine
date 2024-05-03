//Core.
#include <Core/Essential/CatalystEssential.h>

//Header file.
#include <Memory/Memory.h>

//STL.
#include <cstring>
#include <stdlib.h>

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