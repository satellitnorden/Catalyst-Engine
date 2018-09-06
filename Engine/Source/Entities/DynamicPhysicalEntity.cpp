//Header file.
#include <Entities/DynamicPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
DynamicPhysicalEntity::DynamicPhysicalEntity() NOEXCEPT
{
	//Set the entity type.
	entityType = EntityType::DynamicPhysical;
}

/*
*	Returns the position of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT DynamicPhysicalEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT DynamicPhysicalEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
NO_DISCARD RESTRICTED Vector3 *const RESTRICT DynamicPhysicalEntity::GetScaleInternal() NOEXCEPT
{
	//Return the scale of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Scale;
}