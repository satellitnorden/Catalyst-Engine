#pragma once

class Memory
{

public:

	/*
	*	Allocates a chunk of memory.
	*/
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
		return AllocateInternal(size);
	}

	/*
	*	Reallocates a chunk of memory previously allocated with Allocate().
	*/
	template <typename TYPE>
	FORCE_INLINE RESTRICTED static NO_DISCARD void *const RESTRICT Reallocate(TYPE *const RESTRICT memory, const uint64 size) NOEXCEPT
	{
		return ReallocateInternal((void *const RESTRICT) memory, size);
	}

	/*
	*	Frees a chunk of memory.
	*/
	template <typename TYPE>
	FORCE_INLINE static void Free(TYPE *const RESTRICT memory) NOEXCEPT
	{
		FreeInternal((void *const RESTRICT) memory);
	}

	/*
	*	Copies memory from the destination to the source.
	*/
	template <typename TYPE_1, typename TYPE_2>
	FORCE_INLINE static void Copy(TYPE_1 *const RESTRICT destination, const TYPE_2 *const RESTRICT source, const uint64 size) NOEXCEPT
	{
		CopyInternal((void *const RESTRICT) destination, (const void *const RESTRICT) source, size);
	}

	/*
	*	Sets a chunk of memory to the specified value.
	*/
	template <typename TYPE>
	FORCE_INLINE static void Set(TYPE *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT
	{
		SetInternal((void *const RESTRICT) destination, value, size);
	}

	/*
	*	Compares two chunks of memory. Returns true if the chunks are identical, false otherwise.
	*/
	template <typename TYPE>
	FORCE_INLINE static NO_DISCARD bool Compare(const TYPE *const RESTRICT first, const TYPE *const RESTRICT second, const uint64 size) NOEXCEPT
	{
		return CompareInternal(first, second, size);
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	/*
	*	Prints a memory string into the given buffer.
	*/
	static void PrintMemoryString(char *buffer, const uint64 buffer_size, const char *const RESTRICT prefix, const uint64 input_bytes) NOEXCEPT;
#endif

private:

	/*
	*	Allocates a chunk of memory internally.
	*/
	RESTRICTED static NO_DISCARD void *const RESTRICT AllocateInternal(const uint64 size) NOEXCEPT;

	/*
	*	Reallocates a chunk of memory previously allocated with Allocate() internally.
	*/
	RESTRICTED static NO_DISCARD void *const RESTRICT ReallocateInternal(void *const RESTRICT memory, const uint64 size) NOEXCEPT;

	/*
	*	Frees internally.
	*/
	static void FreeInternal(void *const RESTRICT memory) NOEXCEPT;

	/*
	*	Copies memory from the destination to the source internally.
	*/
	static void CopyInternal(void *const RESTRICT destination, const void *const RESTRICT source, const uint64 size) NOEXCEPT;

	/*
	*	Sets a chunk of memory to the specified value internally.
	*/
	static void SetInternal(void *const RESTRICT destination, const byte value, const uint64 size) NOEXCEPT;

	/*
	*	Compares two chunks of memory. Returns true if the chunks are identical, false otherwise internally.
	*/
	static NO_DISCARD bool CompareInternal(const void *const RESTRICT first, const void *const RESTRICT second, const uint64 size) NOEXCEPT;

};