#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* const RESTRICT EntityCreationSystem::CreateEntity(Arguments&&... arguments) NOEXCEPT
{
	_AllocatorLock.WriteLock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.WriteUnlock();

	EntityClass *const RESTRICT newEntity{ new (memory) EntityClass(std::forward<Arguments>(arguments)...) };

	_EntitiesLock.WriteLock();
	_Entities.EmplaceSlow(newEntity);
	_EntitiesLock.WriteUnlock();

	return newEntity;
}

/*
*	Creates initialization data for an entity.
*/
template <typename Type>
RESTRICTED Type* const RESTRICT EntityCreationSystem::CreateInitializationData() NOEXCEPT
{
	void* const RESTRICT memory{ Memory::GlobalPoolAllocate<sizeof(Type)>() };

	new (memory) Type;

	return static_cast<Type* const RESTRICT>(memory);
}

/*
*	Destroys initialization data for an entity.
*/
template <typename Type>
void EntityCreationSystem::DestroyInitializationData(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	Memory::GlobalPoolDeAllocate<sizeof(Type)>(data);
}