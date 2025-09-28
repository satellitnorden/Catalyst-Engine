//Header file.
#include <Systems/MemorySystem.h>

/*
*	Updates the memory system.
*/
void MemorySystem::Update(const UpdatePhase phase) NOEXCEPT
{
	//Reset the frame allocator.
	_FrameAllocator.Reset();
}