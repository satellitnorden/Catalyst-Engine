#pragma once

namespace MemoryUtilities
{
	/*
	*	Allocates a chunk of memory.
	*/
	RESTRICTED static void *const RESTRICT AllocateMemory(const uint64 size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with AllocateMemory().
	*/
	template <typename Type>
	RESTRICTED static void *const RESTRICT ReallocateMemory(Type *const RESTRICT memory, const uint64 size) NOEXCEPT
	{
		return realloc((void *const RESTRICT) memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	template <typename Type>
	static void FreeMemory(Type *const RESTRICT memory) NOEXCEPT
	{
		free((void *const RESTRICT) memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	template <typename Type1, typename Type2>
	static void CopyMemory(Type1 *const RESTRICT destination, const Type2 *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		memcpy((void *const RESTRICT) destination, (const void *const RESTRICT) source, size);
	}

	/*
	*	Sets a chunk of memory to the specified value.
	*/
	template <typename Type>
	static void SetMemory(Type *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
	{
		memset((void *const RESTRICT) destination, value, size);
	}

}