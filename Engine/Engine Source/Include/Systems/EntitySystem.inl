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
RESTRICTED EntityClass *const RESTRICT EntitySystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) const NOEXCEPT
{
	EntityClass *const RESTRICT newChild{ new EntityClass(std::forward<Arguments>(arguments)...) };

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
	void* const RESTRICT memory{ MemoryUtilities::AllocateMemory(sizeof(InitializationDataClass)) };

	new (memory) InitializationDataClass();

	return static_cast<InitializationDataClass* const RESTRICT>(memory);
}

/*
*	Destroys initialization data for an entity.
*/
template <class InitializationDataClass>
void EntitySystem::DestroyInitializationData(void* const RESTRICT data) NOEXCEPT
{
	static_cast<InitializationDataClass* const RESTRICT>(data)->~InitializationDataClass();

	MemoryUtilities::FreeMemory(data);
}