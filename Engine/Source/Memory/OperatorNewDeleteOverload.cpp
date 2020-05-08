//Core.
#include <Core/Essential/CatalystEssential.h>

/*
*	New/delete operator overloads.
*/
void *operator new(size_t size)
{
	return Memory::Allocate(size);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	return Memory::Allocate(size);
}

void operator delete(void *memory)
{
	return Memory::Free(memory);
}