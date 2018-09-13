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
	_ComponentsIndex = ComponentManager::GetNewPointLightComponentsIndex(this);
}

/*
*	Sets whether or not this point light is enabled.
*/
void PointLightEntity::SetEnabled(const bool newEnabled) NOEXCEPT
{
	//Set whether or not this point light is enabled.
	ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._Enabled = newEnabled;
}

/*
*	Sets the attenuation distance.
*/
void PointLightEntity::SetAttenuationDistance(const float newAttenuationDistance) NOEXCEPT
{
	//Set the attenuation distance.
	ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._AttenuationDistance = newAttenuationDistance;
}

/*
*	Sets the intensity.
*/
void PointLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity.
	ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._Intensity = newIntensity;
}

/*
*	Sets the color.
*/
void PointLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{
	//Set the color.
	ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._Color = newColor;
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT PointLightEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetPointLightPointLightComponents()[_ComponentsIndex]._Position;
}