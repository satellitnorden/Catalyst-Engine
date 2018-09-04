//Header file.
#include <Entities/VegetationEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
VegetationEntity::VegetationEntity() NOEXCEPT
{
	//Get a new components index.
	_ComponentsIndex = ComponentManager::GetNewVegetationComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& VegetationEntity::GetPosition() const NOEXCEPT
{
	//Vegetation entities has no position.
	return Entity::_DefaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& VegetationEntity::GetRotation() const NOEXCEPT
{
	//Vegetation entities has no rotation.
	return Entity::_DefaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& VegetationEntity::GetScale() const NOEXCEPT
{
	//Vegetation entities has no scale.
	return Entity::_DefaultScale;
}

/*
*	Moves this entity.
*/
void VegetationEntity::Move(const Vector3 &moveVector) NOEXCEPT
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
void VegetationEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void VegetationEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : _Children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this vegetation entity.
*/
void VegetationEntity::Initialize(const VegetationMaterial &material, const DynamicArray<VegetationTransformation> &transformations, const VegetationProperties &properties) NOEXCEPT
{
	//Initialize this vegetation entity via the rendering system.
	RenderingSystem::Instance->InitializeVegetationEntity(*this, material, transformations, properties);
}