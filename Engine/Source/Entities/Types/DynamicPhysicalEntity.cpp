//Header file.
#include <Entities/Types/DynamicPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
DynamicPhysicalEntity::DynamicPhysicalEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::DynamicPhysical;
}

/*
*	Returns the axis aligned bounding box for this dynamic physical entity in model space.
*/
RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT DynamicPhysicalEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	//Return the axis aligned bounding box for this dynamic physical entity in model space.
	return &ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_ComponentsIndex]._ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the axis aligned bounding box for this dynamic physical entity in world space.
*/
RESTRICTED NO_DISCARD AxisAlignedBoundingBox *const RESTRICT DynamicPhysicalEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	//Returns the axis aligned bounding box for this dynamic physical entity in world space.
	return &ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the position of this entity.
*/
RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT DynamicPhysicalEntity::GetPositionInternal() NOEXCEPT
{
	//Return the position of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Position;
}

/*
*	Returns the rotation of this entity.
*/
RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT DynamicPhysicalEntity::GetRotationInternal() NOEXCEPT
{
	//Return the rotation of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Rotation;
}

/*
*	Returns the scale of this entity.
*/
RESTRICTED NO_DISCARD Vector3<float> *const RESTRICT DynamicPhysicalEntity::GetScaleInternal() NOEXCEPT
{
	//Return the scale of this entity.
	return &ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex]._Scale;
}

/*
*	Updates the world space axis-aligned bounding box.
*/
void DynamicPhysicalEntity::UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	AxisAlignedBoundingBox *const RESTRICT modelSpaceAxisAlignedBoundingBox{ GetModelSpaceAxisAlignedBoundingBox() };

	if (!modelSpaceAxisAlignedBoundingBox)
	{
		return;
	}

	AxisAlignedBoundingBox *const RESTRICT worldSpaceAxisAlignedBoundingBox{ GetWorldSpaceAxisAlignedBoundingBox() };

	if (!worldSpaceAxisAlignedBoundingBox)
	{
		return;
	}

	const Vector3<float> *const RESTRICT position{ GetPositionInternal() };
	const Vector3<float> *const RESTRICT scale{ GetScaleInternal() };

	worldSpaceAxisAlignedBoundingBox->_Minimum = modelSpaceAxisAlignedBoundingBox->_Minimum * *scale + *position;
	worldSpaceAxisAlignedBoundingBox->_Maximum = modelSpaceAxisAlignedBoundingBox->_Maximum * *scale + *position;
}