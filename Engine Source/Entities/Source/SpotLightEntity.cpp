//Header file.
#include <SpotLightEntity.h>

//Components.
#include <ComponentManager.h>

//Define the entity class.
DEFINE_ENTITY_CLASS(SpotLightEntity);

/*
*	Default constructor.
*/
SpotLightEntity::SpotLightEntity() CATALYST_NOEXCEPT
{
	//Get a new components index.
	componentsIndex = ComponentManager::GetNewSpotLightEntitiesEntityComponentsIndex();

	//Add this spot light entity to the universal container.
	Instances.Emplace(this);
}

/*
*	Default destructor.
*/
SpotLightEntity::~SpotLightEntity() CATALYST_NOEXCEPT
{
	//Remove this spot light entity from the universal container.
	Instances.Erase(this);
}

/*
*	Returns the position of this entity.
*/
Vector3& SpotLightEntity::GetPosition() CATALYST_NOEXCEPT
{
	//Return the position of this entity.
	return ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].position;
}

/*
*	Returns the rotation of this entity.
*/
Vector3& SpotLightEntity::GetRotation() CATALYST_NOEXCEPT
{
	//Return the rotation of this entity.
	return ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].rotation;
}

/*
*	Returns the scale of this entity.
*/
Vector3& SpotLightEntity::GetScale() CATALYST_NOEXCEPT
{
	//Spot light entities has no scale.
	static Vector3 defaultScale{ 1.0f, 1.0f, 1.0f };

	return defaultScale;
}

/*
*	Moves this entity.
*/
void SpotLightEntity::Move(const Vector3 &moveVector) CATALYST_NOEXCEPT
{
	//Move this entity.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].position += moveVector;

	//Move all children.
	for (auto child : children)
	{
		child->Move(moveVector);
	}
}

/*
*	Rotates this entity.
*/
void SpotLightEntity::Rotate(const Vector3 &rotateVector) CATALYST_NOEXCEPT
{
	//Rotate this entity.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].rotation += rotateVector;

	//Rotate all children.
	for (auto child : children)
	{
		child->Rotate(rotateVector);
	}
}

/*
*	Scales this entity.
*/
void SpotLightEntity::Scale(const Vector3 &scaleVector) CATALYST_NOEXCEPT
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
bool SpotLightEntity::GetEnabled() const CATALYST_NOEXCEPT
{
	//Return whether or not this spot light is enabled.
	return ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].enabled;
}

/*
*	Sets whether or not this spot light is enabled.
*/
void SpotLightEntity::SetEnabled(const bool newEnabled) CATALYST_NOEXCEPT
{
	//Set whether or not this spot light is enabled.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void SpotLightEntity::SetAttenuationDistance(const float newAttenuationDistance) CATALYST_NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].attenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void SpotLightEntity::SetIntensity(const float newIntensity) CATALYST_NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].intensity = newIntensity;
}

/*
*	Sets the inner cutoff angle.
*/
void SpotLightEntity::SetInnerCutoffAngle(const float newInnerCutoffAngle) CATALYST_NOEXCEPT
{
	//Set the inner cutoff angle.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].innerCutoffAngle = newInnerCutoffAngle;
}

/*
*	Sets the outer cutoff angle.
*/
void SpotLightEntity::SetOuterCutoffAngle(const float newOuterCutoffAngle) CATALYST_NOEXCEPT
{
	//Set the outer cutoff angle.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].outerCutoffAngle = newOuterCutoffAngle;
}

/*
*	Sets the color.
*/
void SpotLightEntity::SetColor(const Vector3 &newColor) CATALYST_NOEXCEPT
{
	//Set the color.
	ComponentManager::GetSpotLightEntitySpotLightComponents()[componentsIndex].color = newColor;
}