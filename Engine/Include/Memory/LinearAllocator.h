#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Utilities/MemoryUtilities.h>

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
		_Memory = MemoryUtilities::AllocateMemory(SIZE);
	}

	/*
	*	Default destructor.
	*/
	~PoolAllocator() NOEXCEPT
	{
		//Free the memory.
		MemoryUtilities::FreeMemory(_Memory);
	}

	/*
	*	Allocates memory.
	*/
	RESTRICTED NO_DISCARD void *const RESTRICT Allocate(const uint64 size) NOEXCEPT
	{
		//Just return the memory at the current index and increase the index.
		return static_cast<void *const RESTRICT>(static_cast<byte *const RESTRICT>(_Memory)[_Index.fetch_add(size)]);
	}

	/*
	*	Resets this allocator.
	*/
	void Reset() NOEXCEPT
	{
		//Reset the index.
		_Index.store(0);
	}

private:

	//The current index.
	std::atomic<uint64> _Index{ 0 };

	//The memory.
	void *RESTRICT _Memory{ nullptr };

};