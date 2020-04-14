//Header file.
#include <Systems/MemorySystem.h>

//Singleton definition.
DEFINE_SINGLETON(MemorySystem);

/*
*	Updates the memory system before the frame begins.
*/
void MemorySystem::PreUpdate() NOEXCEPT
{
	//Reset the frame allocator.
	_FrameAllocator.Reset();
}