//Header file.
#include <InstancedPhysicalEntity.h>

//Components.
#include <ComponentManager.h>

//Systems.
#include <RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(InstancedPhysicalEntity);

/*
*	Default constructor.
*/
InstancedPhysicalEntity::InstancedPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewInstancedPhysicalComponentsIndex();

	//Add this instanced physical entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
InstancedPhysicalEntity::~InstancedPhysicalEntity() NOEXCEPT
{
	//Remove this instanced physical entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetPosition() NOEXCEPT
{
	//Instanced physical entities has no position.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetRotation() NOEXCEPT
{
	//Instanced physical entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& InstancedPhysicalEntity::GetScale() NOEXCEPT
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