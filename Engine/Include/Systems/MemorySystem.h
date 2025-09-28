#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/Spinlock.h>

//Memory.
#include <Memory/FrameAllocator.h>
#include <Memory/PoolAllocator.h>

//Systems.
#include <Systems/System.h>

class MemorySystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		MemorySystem,
		SYSTEM_INITIALIZE()
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE MemorySystem() NOEXCEPT
	{

	}

	/*
	*	Allocates from a heap specific to the current frame.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT FrameAllocate(const uint64 size) NOEXCEPT
	{
		SCOPED_LOCK(_FrameAllocatorLock);

		return _FrameAllocator.Allocate<TYPE>(size);
	}

	/*
	*	Allocates from a heap specific to the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT TypeAllocate() NOEXCEPT;

	/*
	*	Frees from a heap specific to the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE void TypeFree(TYPE *const RESTRICT memory) NOEXCEPT;

private:

	//The frame allocator.
	FrameAllocator<1'024 * 1'024 * 4> _FrameAllocator;

	//The frame allocator lock.
	Spinlock _FrameAllocatorLock;

	/*
	*	Updates the memory system before the frame begins.
	*/
	void PreUpdate() NOEXCEPT;

	/*
	*	Returns the  pool allocator specific to the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD PoolAllocator<sizeof(TYPE)>* const RESTRICT TypePoolAllocator() NOEXCEPT;

	/*
	*	Returns the pool allocator lock specific to the given type.
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED NO_DISCARD Spinlock* const RESTRICT TypePoolAllocatorLock() NOEXCEPT;

};

//Templated function definitions.
#include <Systems/MemorySystem.inl>