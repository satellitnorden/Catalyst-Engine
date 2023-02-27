#pragma once

/*
*	Creates a new entity.
*/
template <class CLASS, class... ARGUMENTS>
RESTRICTED CLASS* const RESTRICT EntitySystem::CreateEntity(ARGUMENTS&&... arguments) NOEXCEPT
{
	_AllocatorLock.Lock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.Unlock();

	Memory::Set(memory, 0, sizeof(Entity));

	CLASS *const RESTRICT newEntity{ new (memory) CLASS(std::forward<ARGUMENTS>(arguments)...) };

	_EntitiesLock.Lock();
	_Entities.Emplace(newEntity);
	_EntitiesLock.Unlock();

	return newEntity;
}

/*
*	Creates initialization data for an entity.
*/
template <typename TYPE>
RESTRICTED TYPE* const RESTRICT EntitySystem::CreateInitializationData() NOEXCEPT
{
	TYPE *const RESTRICT memory{ MemorySystem::Instance->TypeAllocate<TYPE>() };

	Memory::Set(memory, 0, sizeof(TYPE));

	new (memory) TYPE();

	return memory;
}

/*
*	Destroys initialization data for an entity.
*/
template <typename TYPE>
void EntitySystem::DestroyInitializationData(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	static_cast<TYPE *const RESTRICT>(data)->~TYPE();
	
	MemorySystem::Instance->TypeFree<TYPE>(static_cast<TYPE *const RESTRICT>(data));
}