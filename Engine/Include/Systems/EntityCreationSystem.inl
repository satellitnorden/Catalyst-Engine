#pragma once

/*
*	Creates a new entity.
*/
template <class CLASS, class... ARGUMENTS>
RESTRICTED CLASS* const RESTRICT EntityCreationSystem::CreateEntity(ARGUMENTS&&... arguments) NOEXCEPT
{
	_AllocatorLock.Lock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.Unlock();

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
RESTRICTED TYPE* const RESTRICT EntityCreationSystem::CreateInitializationData() NOEXCEPT
{
	void* const RESTRICT memory{ Memory::Allocate(sizeof(TYPE)) };

	new (memory) TYPE();

	return static_cast<TYPE* const RESTRICT>(memory);
}

/*
*	Destroys initialization data for an entity.
*/
template <typename TYPE>
void EntityCreationSystem::DestroyInitializationData(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	static_cast<TYPE *const RESTRICT>(data)->~TYPE();
	Memory::Free(data);
}