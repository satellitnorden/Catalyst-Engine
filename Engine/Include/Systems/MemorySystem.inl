/*
*	Allocates from a heap specific to the given type.
*/
template <typename TYPE>
FORCE_INLINE RESTRICTED NO_DISCARD TYPE *const RESTRICT MemorySystem::TypeAllocate() NOEXCEPT
{
	SCOPED_LOCK(*TypePoolAllocatorLock<TYPE>());
	
	return static_cast<TYPE *const RESTRICT>(TypePoolAllocator<TYPE>()->Allocate());
}

/*
*	Frees from a heap specific to the given type.
*/
template <typename TYPE>
FORCE_INLINE void MemorySystem::TypeFree(TYPE *const RESTRICT memory) NOEXCEPT
{
	SCOPED_LOCK(*TypePoolAllocatorLock<TYPE>());

	TypePoolAllocator<TYPE>()->Free(memory);
}

/*
*	Returns the  pool allocator specific to the given type.
*/
template <typename TYPE>
FORCE_INLINE RESTRICTED NO_DISCARD PoolAllocator<sizeof(TYPE)> *const RESTRICT MemorySystem::TypePoolAllocator() NOEXCEPT
{
	static PoolAllocator<sizeof(TYPE)> allocator;

	return &allocator;
}

/*
*	Returns the pool allocator lock specific to the given type.
*/
template <typename TYPE>
FORCE_INLINE RESTRICTED NO_DISCARD Spinlock *const RESTRICT MemorySystem::TypePoolAllocatorLock() NOEXCEPT
{
	static Spinlock lock;

	return &lock;
}