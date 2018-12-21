#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Utilities/MemoryUtilities.h>

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
		return MemoryUtilities::AllocateMemory(size);
	}

	/*
	*	The deallocate function.
	*/
	void deallocate(void* ptr) override
	{
		MemoryUtilities::FreeMemory(ptr);
	}

};