#pragma once

namespace MemoryUtilities
{
	/*
	*	Allocates a chunk of memory.
	*/
	RESTRICTED static void* AllocateMemory(const uint64 size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with AllocateMemory().
	*/
	RESTRICTED static void* ReallocateMemory(void *const RESTRICT memory, const uint64 size) NOEXCEPT
	{
		return realloc(memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	static void FreeMemory(void *const RESTRICT memory) NOEXCEPT
	{
		free(memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	static void CopyMemory(void *const RESTRICT destination, const void *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		memcpy(destination, source, size);
	}

	/*
	*	Sets a chunk of memory to the specified value.
	*/
	static void SetMemory(void *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
	{
		memset(destination, value, size);
	}

}