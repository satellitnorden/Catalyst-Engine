//Header file.
#include <PointLightEntity.h>

//Components.
#include <ComponentManager.h>

//Math.
#include <Vector3.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(PointLightEntity);

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewPointLightEntitiesEntityComponentsIndex();

	//Add this point light entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
PointLightEntity::~PointLightEntity() CATALYST_NOEXCEPT
{
	//Remove this point light entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& PointLightEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& PointLightEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Point light entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& PointLightEntity::GetScale() CATALYST_NOEXCEPT
{
	//Point light entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void PointLightEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void PointLightEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
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
void PointLightEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Sets whether or not this point light is enabled.
*/
void PointLightEntity::SetEnabled(const bool newEnabled) CATALYST_NOEXCEPT
{
	//Set whether or not this point light is enabled.
	ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void PointLightEntity::SetAttenuationDistance(const float newAttenuationDistance) CATALYST_NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].attenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void PointLightEntity::SetIntensity(const float newIntensity) CATALYST_NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the color.
*/
void PointLightEntity::SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT
{
	//Set the color.
	ComponentManager::GetPointLightEntityPointLightComponents()[componentsIndex].color = newColor;
}