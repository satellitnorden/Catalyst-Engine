//Header file.
#include <Entities/PointLightEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/Vector3.h>

/*
*	Default constructor.
*/
PointLightEntity::PointLightEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewPointLightComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& PointLightEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetPointLightPointLightComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& PointLightEntity::GetRotation() const NOEXCEPT
{
	//Point light entities has no rotation.
	return Entity::defaultRotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& PointLightEntity::GetScale() const NOEXCEPT
{
	//Point light entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void PointLightEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetPointLightPointLightComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void PointLightEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
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
void PointLightEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
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
void PointLightEntity::SetEnabled(const bool newEnabled) NOEXCEPT
{
	//Set whether or not this point light is enabled.
	ComponentManager::GetPointLightPointLightComponents()[componentsIndex].enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void PointLightEntity::SetAttenuationDistance(const float newAttenuationDistance) NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetPointLightPointLightComponents()[componentsIndex].attenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void PointLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetPointLightPointLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the color.
*/
void PointLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{
	//Set the color.
	ComponentManager::GetPointLightPointLightComponents()[componentsIndex].color = newColor;
}