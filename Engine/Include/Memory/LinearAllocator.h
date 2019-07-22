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
	LinearAllocator() NOEXCEPT
	{
		//Allocate the memory.
		_Memory = Memory::Allocate(SIZE);
	}

	/*
	*	Default destructor.
	*/
	~LinearAllocator() NOEXCEPT
	{
		//Free the memory.
		Memory::Free(_Memory);
	}

	/*
	*	Allocates memory.
	*/
	RESTRICTED NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
		ASSERT(_Index.load() + size <= SIZE, "Linear allocator overflow!");

		//Just return the memory at the current index and increase the index.
		return static_cast<void *const RESTRICT>(static_cast<byte *const RESTRICT>(_Memory) + _Index.fetch_add(size));
	}

private:

	//The current index.
	std::atomic<uint64> _Index{ 0 };

	//The memory.
	void *RESTRICT _Memory{ nullptr };

};