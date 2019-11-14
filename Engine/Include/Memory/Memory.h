#pragma once

//Memory.
#include <Memory/DestructorPointer.h>
#include <Memory/LinearAllocator.h>
#include <Memory/PoolAllocator.h>
#include <Memory/UniquePointer.h>

//Multithreading.
#include <Multithreading/ScopedWriteLock.h>
#include <Multithreading/Spinlock.h>

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
	*	Allocates an aligned chunk of memory.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT AllocateAligned(const uint64 alignment, const uint64 size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with Allocate().
	*/
	template <typename Type>
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT Reallocate(Type *const RESTRICT memory, const uint64 size) NOEXCEPT
	{
		return realloc((void *const RESTRICT) memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	template <typename Type>
	FORCE_INLINE static void Free(Type *const RESTRICT memory) NOEXCEPT
	{
		free((void *const RESTRICT) memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	template <typename Type1, typename Type2>
	FORCE_INLINE static void Copy(Type1 *const RESTRICT destination, const Type2 *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		memcpy((void *const RESTRICT) destination, (const void *const RESTRICT) source, size);
	}

	/*
	*	Sets a chunk of memory to the specified value.
	*/
	template <typename Type>
	FORCE_INLINE static void Set(Type *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
	{
		memset((void *const RESTRICT) destination, value, size);
	}

	/*
	*	Returns the global linear allocator.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> *const RESTRICT GlobalLinearAllocator() NOEXCEPT
	{
		static LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> allocator;

		return &allocator;
	}

	/*
	*	Returns a thread safe pool allocation with the given size from the global pool allocator.
	*/
	template <uint64 SIZE>
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT GlobalPoolAllocate() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ *GlobalPoolAllocatorLock<SIZE>() };

		return GlobalPoolAllocator<SIZE>()->Allocate();
	}

	/*
	*	Returns a thread safe pool allocation with the given size from the global pool allocator.
	*/
	template <uint64 SIZE>
	FORCE_INLINE static void GlobalPoolDeAllocate(void *const RESTRICT memory) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ *GlobalPoolAllocatorLock<SIZE>() };

		GlobalPoolAllocator<SIZE>()->DeAllocate(memory);
	}

private:

	/*
	*	Returns the global pool allocator lock with the given size.
	*/
	template <uint64 SIZE>
	FORCE_INLINE RESTRICTED static NO_DISCARD Spinlock *const RESTRICT GlobalPoolAllocatorLock() NOEXCEPT
	{
		static Spinlock lock;

		return &lock;
	}

	/*
	*	Returns the global pool allocator with the given size.
	*/
	template <uint64 SIZE>
	FORCE_INLINE RESTRICTED static NO_DISCARD PoolAllocator<SIZE> *const RESTRICT GlobalPoolAllocator() NOEXCEPT
	{
		static PoolAllocator<SIZE> allocator;

		return &allocator;
	}

};