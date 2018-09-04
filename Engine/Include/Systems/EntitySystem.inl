#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* const RESTRICT EntitySystem::CreateEntity(Arguments&&... arguments) NOEXCEPT
{
	EntityClass *const RESTRICT newEntity{ new EntityClass(std::forward<Arguments>(arguments)...) };

	_Entities.EmplaceSlow(newEntity);

	return newEntity;
}

/*
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass *const RESTRICT EntitySystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) NOEXCEPT
{
	EntityClass *const RESTRICT newChild{ new EntityClass(std::forward<Arguments>(arguments)...) };

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	_Entities.EmplaceSlow(newChild);

	return newChild;
}

/*
*	Creates initialization data for an entity.
*/
template <typename Type>
RESTRICTED Type* const RESTRICT EntitySystem::CreateInitializationData() NOEXCEPT
{
	void* const RESTRICT memory{ MemoryUtilities::AllocateMemory(sizeof(Type)) };

	new (memory) Type;

	return static_cast<Type* const RESTRICT>(memory);
}

/*
*	Destroys initialization data for an entity.
*/
template <typename Type>
void EntitySystem::DestroyInitializationData(void* const RESTRICT data) NOEXCEPT
{
	static_cast<Type* const RESTRICT>(data)->~Type();

	MemoryUtilities::FreeMemory(data);
}