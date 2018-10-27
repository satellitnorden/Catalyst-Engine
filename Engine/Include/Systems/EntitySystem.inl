#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* const RESTRICT EntityCreationSystem::CreateEntity(Arguments&&... arguments) NOEXCEPT
{
	_AllocatorLock.Lock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.Unlock();

	EntityClass *const RESTRICT newEntity{ new (memory) EntityClass(std::forward<Arguments>(arguments)...) };

	_Entities.EmplaceSlow(newEntity);

	return newEntity;
}

/*
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass *const RESTRICT EntityCreationSystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) NOEXCEPT
{
	_AllocatorLock.Lock();
	void *const RESTRICT memory{ _Allocator.Allocate() };
	_AllocatorLock.Unlock();

	EntityClass *const RESTRICT newChild{ new (memory) EntityClass(std::forward<Arguments>(arguments)...) };

	parentEntity->_Children.EmplaceSlow(newChild);
	newChild->_Parent = parentEntity;

	_Entities.EmplaceSlow(newChild);

	return newChild;
}

/*
*	Creates initialization data for an entity.
*/
template <typename Type>
RESTRICTED Type* const RESTRICT EntityCreationSystem::CreateInitializationData() NOEXCEPT
{
	void* const RESTRICT memory{ MemoryUtilities::GlobalPoolAllocate<sizeof(Type)>() };

	new (memory) Type;

	return static_cast<Type* const RESTRICT>(memory);
}

/*
*	Destroys initialization data for an entity.
*/
template <typename Type>
void EntityCreationSystem::DestroyInitializationData(EntityInitializationData* const RESTRICT data) NOEXCEPT
{
	MemoryUtilities::GlobalPoolDeAllocate<sizeof(Type)>(data);
}