#pragma once

/*
*	Creates a new entity.
*/
template <class CLASS, class... ARGUMENTS>
RESTRICTED CLASS* const RESTRICT EntityCreationSystem::CreateEntity(ARGUMENTS&&... arguments) NOEXCEPT
{
	_AllocatorLock.WriteLock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.WriteUnlock();

	CLASS *const RESTRICT newEntity{ new (memory) CLASS(std::forward<ARGUMENTS>(arguments)...) };

	_EntitiesLock.WriteLock();
	_Entities.EmplaceSlow(newEntity);
	_EntitiesLock.WriteUnlock();

	return newEntity;
}

/*
*	Creates initialization data for an entity.
*/
template <typename TYPE>
RESTRICTED TYPE* const RESTRICT EntityCreationSystem::CreateInitializationData() NOEXCEPT
{
	void* const RESTRICT memory{ Memory::GlobalPoolAllocate<sizeof(TYPE)>() };

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
	Memory::GlobalPoolDeAllocate<sizeof(TYPE)>(data);
}