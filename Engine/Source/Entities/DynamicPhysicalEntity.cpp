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
	return ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& DynamicPhysicalEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& DynamicPhysicalEntity::GetScale() const NOEXCEPT
{
	//Return the scale of this entity.
	return ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Scale;
}

/*
*	Moves this entity.
*/
void DynamicPhysicalEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Position += moveVector;

	//Move all children.
	for (auto child : _Children)
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
	ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Rotation += rotateVector;

	//Rotate all children.
	for (auto child : _Children)
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
	ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Scale *= scaleVector;

	//Scale all children.
	for (auto child : _Children)
	{
		child->Scale(scaleVector);
	}
}