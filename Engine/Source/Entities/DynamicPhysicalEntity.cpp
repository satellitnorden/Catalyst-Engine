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
	_EntityType = EntityType::DynamicPhysical;
}

/*
*	Returns the axis aligned bounding box for this dynamic physical entity in model space.
*/
NO_DISCARD RESTRICTED const AxisAlignedBoundingBox *const RESTRICT DynamicPhysicalEntity::GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	//Return the axis aligned bounding box for this dynamic physical entity in model space.
	return &ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_ComponentsIndex]._ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the axis aligned bounding box for this dynamic physical entity in world space.
*/
NO_DISCARD RESTRICTED const AxisAlignedBoundingBox *const RESTRICT DynamicPhysicalEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	//Returns the axis aligned bounding box for this dynamic physical entity in world space.
	return &ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
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