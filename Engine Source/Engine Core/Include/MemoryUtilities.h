#pragma once

namespace MemoryUtilities
{
	/*
	*	Allocates a chunk of memory.
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED static void* AllocateMemory(const size_t size) CATALYST_NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with AllocateMemory().
	*/
	CATALYST_NOALIAS CATALYST_RESTRICTED static void* ReallocateMemory(void *const CATALYST_RESTRICT memory, const size_t size) CATALYST_NOEXCEPT
	{
		return realloc(memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	CATALYST_NOALIAS static void FreeMemory(void *const CATALYST_RESTRICT memory) CATALYST_NOEXCEPT
	{
		free(memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	CATALYST_NOALIAS static void CopyMemory(void *const CATALYST_RESTRICT destination, const void *const CATALYST_RESTRICT source, const size_t size) CATALYST_NOEXCEPT
	{
		memcpy(destination, source, size);
	}

}