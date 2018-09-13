//Header file.
#include <Entities/DirectionalLightEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Math.
#include <Math/Vector3.h>

/*
*	Default constructor.
*/
DirectionalLightEntity::DirectionalLightEntity() NOEXCEPT
{
	//Get a new component index.
	_ComponentsIndex = ComponentManager::GetNewDirectionalLightComponentsIndex(this);
}

/*
*	Returns the rotation of this entity.
*/
RESTRICTED NO_DISCARD Vector3 *const RESTRICT DirectionalLightEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetDirectionalLightDirectionalLightComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Sets the intensity of this directional light entity.
*/
void DirectionalLightEntity::SetIntensity(const float newIntensity) NOEXCEPT
{
	//Set the intensity of this directional light entity.
	ComponentManager::GetDirectionalLightDirectionalLightComponents()[_ComponentsIndex]._Intensity = newIntensity;
}

/*
*	Sets the color of this directional light entity.
*/
void DirectionalLightEntity::SetColor(const Vector3 &newColor) NOEXCEPT
{
	//Set the color of this directional light entity.
	ComponentManager::GetDirectionalLightDirectionalLightComponents()[_ComponentsIndex]._Color = newColor;
}