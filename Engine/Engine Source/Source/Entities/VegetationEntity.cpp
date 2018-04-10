//Header file.
#include <Entities/VegetationEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Systems.
#include <Systems/RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(VegetationEntity);

/*
*	Default constructor.
*/
VegetationEntity::VegetationEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewVegetationComponentsIndex();

	//Add this vegetation entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
VegetationEntity::~VegetationEntity() NOEXCEPT
{
	//Remove this vegetation entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& VegetationEntity::GetPosition() const NOEXCEPT
{
	//Vegetation entities has no position.
	return Entity::defaultPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& VegetationEntity::GetRotation() const NOEXCEPT
{
	//Vegetation entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& VegetationEntity::GetScale() const NOEXCEPT
{
	//Vegetation entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void VegetationEntity::Move(const Vector3 &moveVector) NOEXCEPT
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
void VegetationEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void VegetationEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this vegetation entity.
*/
void VegetationEntity::Initialize(const DynamicArray<VegetationTransformation> &transformations) NOEXCEPT
{
	//Initialize this vegetation entity via the rendering system.
	RenderingSystem::Instance->InitializeVegetationEntity(*this, transformations);
}