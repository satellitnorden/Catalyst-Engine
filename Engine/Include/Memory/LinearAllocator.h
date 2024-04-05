#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <uint64 SIZE>
class LinearAllocator final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE LinearAllocator() NOEXCEPT
	{
		//Allocate the memory.
		_Memory = malloc(SIZE);
	}

	/*
	*	Default destructor.
	*/
	FORCE_INLINE ~LinearAllocator() NOEXCEPT
	{
		//Free the memory.
		free(_Memory);
	}

	/*
	*	Allocates memory.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
#if !defined(CATALYST_CONFIGURATION_FINAL)
		if ((_Index + size) > SIZE)
		{
			ASSERT(false, "Increase size!");
		}
#endif

		//Just return the memory at the current index and increase the index.
		void *const RESTRICT memory{ static_cast<void *const RESTRICT>(static_cast<byte *const RESTRICT>(_Memory) + _Index) };
		_Index += size;

		return memory;
	}

	/*
	*	Resets this allocator.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		_Index = 0;
	}

private:

	//The current index.
	uint64 _Index{ 0 };

	//The memory.
	void *RESTRICT _Memory{ nullptr };

};