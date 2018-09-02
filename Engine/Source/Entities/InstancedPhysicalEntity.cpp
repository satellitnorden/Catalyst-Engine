//Header file.
#include <Entities/InstancedPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
InstancedPhysicalEntity::InstancedPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewInstancedPhysicalComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetPosition() const NOEXCEPT
{
	//Instanced physical entities has no position.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetRotation() const NOEXCEPT
{
	//Instanced physical entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetScale() const NOEXCEPT
{
	//Instanced physical entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void InstancedPhysicalEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void InstancedPhysicalEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void InstancedPhysicalEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this instanced physical entity.
*/
void InstancedPhysicalEntity::Initialize(const PhysicalModel &model, const DynamicArray<Matrix4> &transformations) NOEXCEPT
{
	//Initialize this instanced physical entity via the rendering system.
	RenderingSystem::Instance->InitializeInstancedPhysicalEntity(*this, model, transformations);
}