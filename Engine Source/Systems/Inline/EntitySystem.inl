#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* EntitySystem::CreateEntity(Arguments&&... arguments) const NOEXCEPT
{
	//EntityClass::Instances.Emplace(std::forward<Arguments>(arguments)...)

	return new EntityClass(std::forward<Arguments>(arguments)...);
}

/*
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
RESTRICTED EntityClass* EntitySystem::CreateChildEntity(Entity *RESTRICT parentEntity, Arguments&&... arguments) const NOEXCEPT
{
	EntityClass *RESTRICT newChild = new EntityClass(std::forward<Arguments>(arguments)...);

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	return newChild;
}