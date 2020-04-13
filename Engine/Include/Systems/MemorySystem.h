#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/ScopedLock.h>
#include <Concurrency/Spinlock.h>

//Memory.
#include <Memory/PoolAllocator.h>

class MemorySystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(MemorySystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE MemorySystem() NOEXCEPT
	{

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