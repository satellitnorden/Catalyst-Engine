//Header file.
#include <Entities/StaticPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
StaticPhysicalEntity::StaticPhysicalEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewStaticPhysicalComponentsIndex(this);
}

/*
*	Initializes this static physical entity.
*/
void StaticPhysicalEntity::Initialize(const PhysicalModel &newModel, const Vector3 &position, const Vector3 &rotation, const Vector3 &scale) NOEXCEPT
{
	//Initialize this physical entity.
	RenderingSystem::Instance->InitializeStaticPhysicalEntity(*this, newModel, position, rotation, scale);
}

/*
*	Returns the position of this entity.
*/
const Vector3& StaticPhysicalEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& StaticPhysicalEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& StaticPhysicalEntity::GetScale() const NOEXCEPT
{
	//Return the scale of this entity.
	return ComponentManager::GetStaticPhysicalTransformComponents()[_ComponentsIndex]._Scale;
}

/*
*	Moves this entity.
*/
void StaticPhysicalEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move all children.
	for (auto child : _Children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void StaticPhysicalEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate all children.
	for (auto child : _Children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void StaticPhysicalEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : _Children)
	{
		child->Scale(scaleVector);
	}
}