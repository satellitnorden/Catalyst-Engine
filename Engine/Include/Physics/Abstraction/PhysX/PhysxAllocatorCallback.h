#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Physics.
#include <Physics/Abstraction/PhysX/PhysxCore.h>

class PhysxAllocatorCallback final : public physx::PxAllocatorCallback
{

public:

	/*
	*	The allocate function.
	*/
	void* allocate(size_t size, const char* typeName, const char* filename, int line) override
	{
		//PhysX memory needs to be 16-byte aligned. (:
		return Memory::AllocateAlignedMemory(16, size);
	}

	/*
	*	The deallocate function.
	*/
	void deallocate(void* ptr) override
	{
		Memory::FreeMemory(ptr);
	}

};