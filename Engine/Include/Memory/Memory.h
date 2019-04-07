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
	constexpr uint64 GLOBAL_LINEAR_ALLOCATOR_SIZE{ 16'384 };
}

class Memory
{

public:

	/*
	*	Allocates a chunk of memory.
	*/
	RESTRICTED static void *const RESTRICT AllocateMemory(const uint64 size) NOEXCEPT
	{
		return malloc(size);
	}

	/*
	*	Allocates an aligned chunk of memory.
	*/
	RESTRICTED static void *const RESTRICT AllocateAlignedMemory(const uint64 alignment, const uint64 size) NOEXCEPT
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

	/*
	*	Returns the global linear allocator.
	*/
	RESTRICTED static NO_DISCARD LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> *const RESTRICT GlobalLinearAllocator() NOEXCEPT
	{
		static LinearAllocator<MemoryConstants::GLOBAL_LINEAR_ALLOCATOR_SIZE> allocator;

		return &allocator;
	}

	/*
	*	Returns a thread safe pool allocation with the given size from the global pool allocator.
	*/
	template <uint64 SIZE>
	RESTRICTED static void *const RESTRICT GlobalPoolAllocate() NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ *GlobalPoolAllocatorLock<SIZE>() };

		return GlobalPoolAllocator<SIZE>()->Allocate();
	}

	/*
	*	Returns a thread safe pool allocation with the given size from the global pool allocator.
	*/
	template <uint64 SIZE>
	static void GlobalPoolDeAllocate(void *const RESTRICT memory) NOEXCEPT
	{
		ScopedWriteLock<Spinlock> scopedLock{ *GlobalPoolAllocatorLock<SIZE>() };

		GlobalPoolAllocator<SIZE>()->DeAllocate(memory);
	}

private:

	/*
	*	Returns the global pool allocator lock with the given size.
	*/
	template <uint64 SIZE>
	RESTRICTED static Spinlock *const RESTRICT GlobalPoolAllocatorLock() NOEXCEPT
	{
		static Spinlock lock;

		return &lock;
	}

	/*
	*	Returns the global pool allocator with the given size.
	*/
	template <uint64 SIZE>
	RESTRICTED static PoolAllocator<SIZE> *const RESTRICT GlobalPoolAllocator() NOEXCEPT
	{
		static PoolAllocator<SIZE> allocator;

		return &allocator;
	}

};