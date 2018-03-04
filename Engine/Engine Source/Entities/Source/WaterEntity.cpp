//Header file.
#include <WaterEntity.h>

//Components.
#include <ComponentManager.h>

//Systems.
#include <RenderingSystem.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(WaterEntity);

/*
*	Default constructor.
*/
WaterEntity::WaterEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewWaterComponentsIndex();

	//Add this water entity to the universal container.
	Instances.EmplaceSlow(this);
}

/*
*	Default destructor.
*/
WaterEntity::~WaterEntity() NOEXCEPT
{
	//Remove this water entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& WaterEntity::GetPosition() NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetWaterComponents()[componentsIndex].waterUniformData.waterWorldPosition;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& WaterEntity::GetRotation() NOEXCEPT
{
	//Water entities has no scale.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& WaterEntity::GetScale() NOEXCEPT
{
	//Water entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void WaterEntity::Move(const Vector3 &moveVector) NOEXCEPT
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
void WaterEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void WaterEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Initializes this water entity.
*/
void WaterEntity::Initialize(const WaterMaterial &waterMaterial, const WaterUniformData &waterUniformData) NOEXCEPT
{
	//Initialize this water entity via the rendering system.
	RenderingSystem::Instance->InitializeWaterEntity(this, waterMaterial, waterUniformData);
}