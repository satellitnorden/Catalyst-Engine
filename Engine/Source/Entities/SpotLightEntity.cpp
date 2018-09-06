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
	_ComponentsIndex = ComponentManager::GetNewSpotLightComponentsIndex(this);
}

/*
*	Returns the position of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT SpotLightEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT SpotLightEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns whether or not this spot light is enabled.
*/
bool SpotLightEntity::GetEnabled() const NOEXCEPT
{
	//Return whether or not this spot light is enabled.
	return ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Enabled;
}

/*
*	Sets whether or not this spot light is enabled.
*/
void SpotLightEntity::SetEnabled(const bool newEnabled) NOEXCEPT
{
	//Set whether or not this spot light is enabled.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void SpotLightEntity::SetAttenuationDistance(const float newAttenuationDistance) NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._AttenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void SpotLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Intensity = newIntensity;
}

/*
*	Sets the inner cutoff angle.
*/
void SpotLightEntity::SetInnerCutoffAngle(const float newInnerCutoffAngle) NOEXCEPT
{
	//Set the inner cutoff angle.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._InnerCutoffAngle = newInnerCutoffAngle;
}

/*
*	Sets the outer cutoff angle.
*/
void SpotLightEntity::SetOuterCutoffAngle(const float newOuterCutoffAngle) NOEXCEPT
{
	//Set the outer cutoff angle.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._OuterCutoffAngle = newOuterCutoffAngle;
}

/*
*	Sets the color.
*/
void SpotLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{
	//Set the color.
	ComponentManager::GetSpotLightSpotLightComponents()[_ComponentsIndex]._Color = newColor;
}