//Header file.
#include <Entities/DynamicPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
DynamicPhysicalEntity::DynamicPhysicalEntity() NOEXCEPT
{
	//Set the entity type.
	entityType = EntityType::DynamicPhysical;
}

/*
*	Returns the position of this entity.
*/
const Vector3& DynamicPhysicalEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& DynamicPhysicalEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& DynamicPhysicalEntity::GetScale() const NOEXCEPT
{
	//Return the scale of this entity.
	return ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].scale;
}

/*
*	Moves this entity.
*/
void DynamicPhysicalEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void DynamicPhysicalEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void DynamicPhysicalEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale this entity.
	ComponentManager::GetDynamicPhysicalTransformComponents()[componentsIndex].scale *= scaleVector;

	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}