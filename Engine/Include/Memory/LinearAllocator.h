#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Atomic.h>

template <uint64 SIZE>
class LinearAllocator final
{

public:

	/*
	*	Default constructor.
	*/
	LinearAllocator() NOEXCEPT
	{
		//Allocate the memory.
		_Memory = malloc(SIZE);
	}

	/*
	*	Default destructor.
	*/
	~LinearAllocator() NOEXCEPT
	{
		//Free the memory.
		free(_Memory);
	}

	/*
	*	Allocates memory.
	*/
	RESTRICTED NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
#if !defined(CATALYST_CONFIGURATION_FINAL)
		if (_Index.load() + size > SIZE)
		{
			CRASH();
		}
#endif

		//Just return the memory at the current index and increase the index.
		return static_cast<void *const RESTRICT>(static_cast<byte *const RESTRICT>(_Memory) + _Index.fetch_add(size));
	}

private:

	//The current index.
	Atomic<uint64> _Index{ 0 };

	//The memory.
	void *RESTRICT _Memory{ nullptr };

};