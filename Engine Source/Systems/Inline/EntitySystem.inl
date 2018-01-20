#pragma once

/*
*	Creates a new entity.
*/
template <class EntityClass>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateEntity() const CATALYST_NOEXCEPT
{
	return new EntityClass();
}

/*
*	Creates a new child entity.
*/
template <class EntityClass>
CATALYST_RESTRICTED EntityClass* EntitySystem::CreateChildEntity(Entity *CATALYST_RESTRICT parentEntity) const CATALYST_NOEXCEPT
{
	EntityClass *CATALYST_RESTRICT newChild = new EntityClass();

	parentEntity->AddChild(newChild);
	newChild->SetParent(parentEntity);

	return newChild;
}