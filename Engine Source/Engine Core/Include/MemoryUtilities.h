#pragma once

namespace MemoryUtilities
{
	/*
	*	Allocates a chunk of memory.
	*/
	RESTRICTED static void* AllocateMemory(const size_t size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with AllocateMemory().
	*/
	RESTRICTED static void* ReallocateMemory(void *const RESTRICT memory, const size_t size) NOEXCEPT
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
	static void CopyMemory(void *const RESTRICT destination, const void *const RESTRICT source, const size_t size) NOEXCEPT
	{
		memcpy(destination, source, size);
	}

}