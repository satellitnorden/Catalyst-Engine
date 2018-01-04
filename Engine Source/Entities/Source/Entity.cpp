//Header file.
#include <Entity.h>

//Static variable definitions.
DynamicArray<Entity *CATALYST_RESTRICT> Entity::entities;

/*
*	Default constructor.
*/
Entity::Entity() CATALYST_NOEXCEPT
{
	//Add this entity to the universal container.
	entities.Emplace(this);
}

/*
*	Default destructor.
*/
Entity::~Entity() CATALYST_NOEXCEPT
{
	//Remove this entity from the universal container.
	for (Entity * CATALYST_RESTRICT entity : entities)
	{
		if (entity == this)
		{
			entity = entities.Back();
			entities.Pop();

			return;
		}
	}
}

/*
*	Returns the world position of this entity.
*/
Vector3 Entity::GetWorldPosition() const CATALYST_NOEXCEPT
{
	Vector3 worldPosition{ localPosition.GetSafe() };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldPosition += parentEntity->GetLocalPosition();

		parentEntity = parentEntity->GetParent();
	}

	return worldPosition;
}

/*
*	Returns the world rotation of this entity.
*/
Vector3 Entity::GetWorldRotation() const CATALYST_NOEXCEPT
{
	Vector3 worldRotation{ localRotation };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldRotation += parentEntity->GetLocalRotation();

		parentEntity = parentEntity->GetParent();
	}

	return worldRotation;
}

/*
*	Returns the world position of this world entity.
*/
Vector3 Entity::GetWorldScale() const CATALYST_NOEXCEPT
{
	Vector3 worldScale{ localScale };
	const Entity *CATALYST_RESTRICT parentEntity = parent;

	while (parentEntity)
	{
		worldScale *= parentEntity->GetLocalScale();

		parentEntity = parentEntity->GetParent();
	}

	return worldScale;
}