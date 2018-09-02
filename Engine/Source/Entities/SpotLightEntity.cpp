//Header file.
#include <Entities/SpotLightEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
SpotLightEntity::SpotLightEntity() NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewSpotLightComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
const Vector3& SpotLightEntity::GetPosition() const NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
const Vector3& SpotLightEntity::GetRotation() const NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
const Vector3& SpotLightEntity::GetScale() const NOEXCEPT
{
	//Spot light entities has no scale.
	return Entity::defaultScale;
}

/*
*	Moves this entity.
*/
void SpotLightEntity::Move(const Vector3 &moveVector) NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void SpotLightEntity::Rotate(const Vector3 &rotateVector) NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void SpotLightEntity::Scale(const Vector3 &scaleVector) NOEXCEPT
{
	//Scale all children.
	for (auto child : children)
	{
		child->Scale(scaleVector);
	}
}

/*
*	Returns whether or not this spot light is enabled.
*/
bool SpotLightEntity::GetEnabled() const NOEXCEPT
{
	//Return whether or not this spot light is enabled.
	return ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].enabled;
}

/*
*	Sets whether or not this spot light is enabled.
*/
void SpotLightEntity::SetEnabled(const bool newEnabled) NOEXCEPT
{
	//Set whether or not this spot light is enabled.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void SpotLightEntity::SetAttenuationDistance(const float newAttenuationDistance) NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].attenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void SpotLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the inner cutoff angle.
*/
void SpotLightEntity::SetInnerCutoffAngle(const float newInnerCutoffAngle) NOEXCEPT
{
	//Set the inner cutoff angle.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].innerCutoffAngle = newInnerCutoffAngle;
}

/*
*	Sets the outer cutoff angle.
*/
void SpotLightEntity::SetOuterCutoffAngle(const float newOuterCutoffAngle) NOEXCEPT
{
	//Set the outer cutoff angle.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].outerCutoffAngle = newOuterCutoffAngle;
}

/*
*	Sets the color.
*/
void SpotLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{
	//Set the color.
	ComponentManager::GetSpotLightSpotLightComponents()[componentsIndex].color = newColor;
}