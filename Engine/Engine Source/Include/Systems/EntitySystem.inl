#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* const RESTRICT EntitySystem::CreateEntity(Arguments&&... arguments) const NOEXCEPT
{
	//EntityClass::Instances.EmplaceSlow(std::forward<Arguments>(arguments)...)

	return new EntityClass(std::forward<Arguments>(arguments)...);
}

/*
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* const RESTRICT EntitySystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) const NOEXCEPT
{
	EntityClass *RESTRICT newChild = new EntityClass(std::forward<Arguments>(arguments)...);

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	return newChild;
}

/*
*	Creates initialization data for an entity.
*/
template <class InitializationDataClass>
RESTRICTED InitializationDataClass* const RESTRICT EntitySystem::CreateInitializationData() NOEXCEPT
{
	const void* const RESTRICT memory{ MemoryUtilities::AllocateMemory(sizeof(InitializationDataClass)) };

	new (memory) InitializationDataClass();

	return memory;
}

/*
*	Destroys initialization data for an entity.
*/
template <class InitializationDataClass>
void EntitySystem::DestroyInitializationData(InitializationDataClass* const RESTRICT data) NOEXCEPT
{
	data->~InitializationDataClass();

	MemoryUtilities::FreeMemory(data);
}