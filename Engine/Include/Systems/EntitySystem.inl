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
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass *const RESTRICT EntityCreationSystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) NOEXCEPT
{
	_AllocatorLock.WriteLock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.WriteUnlock();

	EntityClass *const RESTRICT newChild{ new (memory) EntityClass(std::forward<Arguments>(arguments)...) };

	parentEntity->_Children.EmplaceSlow(newChild);
	newChild->_Parent = parentEntity;

	_EntitiesLock.WriteLock();
	_Entities.EmplaceSlow(newChild);
	_EntitiesLock.WriteUnlock();

	return newChild;
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