//Header file.
#include <Systems/MemorySystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(MemorySystem);

/*
*	Initializes the memory system.
*/
void MemorySystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<MemorySystem *const RESTRICT>(arguments)->PreUpdate();
	},
	this,
	UpdatePhase::PRE,
	UpdatePhase::ENTITY,
	false);
}

/*
*	Updates the memory system before the frame begins.
*/
void MemorySystem::PreUpdate() NOEXCEPT
{
	//Reset the frame allocator.
	_FrameAllocator.Reset();
}