#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass, class... Arguments>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateEntity(Arguments&&... arguments) const CATALYST_NOEXCEPT
{
	//EntityClass::Instances.Emplace(std::forward<Arguments>(arguments)...)

	return new EntityClass(std::forward<Arguments>(arguments)...);
}

/*
*	Creates a new child entity.
*/
template <class EntityClass, class... Arguments>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateChildEntity(Entity *CATALYST_RESTRICT parentEntity, Arguments&&... arguments) const CATALYST_NOEXCEPT
{
	EntityClass *CATALYST_RESTRICT newChild = new EntityClass(std::forward<Arguments>(arguments)...);

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	return newChild;
}