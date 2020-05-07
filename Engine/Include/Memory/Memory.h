#pragma once

//Concurrency.
#include <Concurrency/ScopedLock.h>

//Memory.
#include <Memory/DestructorPointer.h>
#include <Memory/LinearAllocator.h>
#include <Memory/UniquePointer.h>

//Memory constants.
namespace MemoryConstants
{
	constexpr uint64 GLOBAL_LINEAR_ALLOCATOR_SIZE{ 262'144 };
}

class Memory
{

public:

	/*
	*	Allocates a chunk of memory.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with Allocate().
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT Reallocate(TYPE *const RESTRICT memory, const uint64 size) NOEXCEPT
	{
		return realloc((void *const RESTRICT) memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	template <typename TYPE>
	FORCE_INLINE static void Free(TYPE *const RESTRICT memory) NOEXCEPT
	{
		free((void *const RESTRICT) memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	template <typename TYPE_1, typename TYPE_2>
	FORCE_INLINE static void Copy(TYPE_1 *const RESTRICT destination, const TYPE_2 *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		memcpy((void *const RESTRICT) destination, (const void *const RESTRICT) source, size);
	}

	/*
	*	Sets a chunk of memory to the specified value.
	*/
	template <typename TYPE>
	FORCE_INLINE static void Set(TYPE *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
	{
		memset((void *const RESTRICT) destination, value, size);
	}

	/*
	*	Compares two chunks of memory. Returns true if the chunks are identical, false otherwise.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD bool Compare(const TYPE *const RESTRICT first, const TYPE *const RESTRICT second, const uint64 size) NOEXCEPT
	{
		return memcmp(first, second, size) == 0;
	}

	/*
	*	Prefetches the memory pointed to by the given pointer.
	*/
	template <typename TYPE>
	FORCE_INLINE static void Prefetch(const TYPE* const RESTRICT pointer) NOEXCEPT;

	/*
	*	Returns the global linear allocator.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> *const RESTRICT GlobalLinearAllocator() NOEXCEPT
	{
		static LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> allocator;

		return &allocator;
	}

};

//Implementation.
#if defined(CATALYST_PLATFORM_WINDOWS)
	#include <Platform/Windows/WindowsMemory.inl>
#endif