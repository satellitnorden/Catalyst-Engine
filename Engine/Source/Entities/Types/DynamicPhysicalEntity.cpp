//Header file.
#include <Entities/Types/DynamicPhysicalEntity.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/Creation/DynamicPhysicalInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
DynamicPhysicalEntity::DynamicPhysicalEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::DynamicPhysical;
}

/*
*	Initializes this entity.
*/
void DynamicPhysicalEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this dynamic physical entity.
	_ComponentsIndex = ComponentManager::GetNewDynamicPhysicalComponentsIndex(this);

	//Cache the data.
	DynamicOutlineRenderComponent &outlineRenderComponent{ ComponentManager::GetDynamicPhysicalDynamicOutlineRenderComponents()[_ComponentsIndex] };
	DynamicPhysicalRenderComponent &physicalRenderComponent{ ComponentManager::GetDynamicPhysicalDynamicPhysicalRenderComponents()[_ComponentsIndex] };
	FrustumCullingComponent &cullingComponent{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents()[_ComponentsIndex] };
	TransformComponent &transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents()[_ComponentsIndex] };
	const DynamicPhysicalInitializationData *const RESTRICT dynamicPhysicalInitializationData{ static_cast<const DynamicPhysicalInitializationData *const RESTRICT>(data) };

	//Initialize the outline render component.
	outlineRenderComponent._PhysicalFlags = dynamicPhysicalInitializationData->_Flags;
	outlineRenderComponent._IsInViewFrustum = true;
	outlineRenderComponent._Buffer = dynamicPhysicalInitializationData->_Model._Buffers[UNDERLYING(LevelOfDetail::High)];
	outlineRenderComponent._IndexOffset = dynamicPhysicalInitializationData->_Model._IndexOffsets[UNDERLYING(LevelOfDetail::High)];
	outlineRenderComponent._IndexCount = dynamicPhysicalInitializationData->_Model._IndexCounts[UNDERLYING(LevelOfDetail::High)];
	outlineRenderComponent._Color = dynamicPhysicalInitializationData->_OutlineColor;

	//Initialize the physical render component.
	physicalRenderComponent._PhysicalFlags = dynamicPhysicalInitializationData->_Flags;
	physicalRenderComponent._IsInViewFrustum = true;
	physicalRenderComponent._Model = dynamicPhysicalInitializationData->_Model;
	physicalRenderComponent._Material = dynamicPhysicalInitializationData->_Material;

	//Initialize the culling component.
	cullingComponent._ModelSpaceAxisAlignedBoundingBox = dynamicPhysicalInitializationData->_Model._AxisAlignedBoundingBoxes[UNDERLYING(LevelOfDetail::High)];

	//Initialize the transform component.
	transformComponent._Position = dynamicPhysicalInitializationData->_Position;
	transformComponent._Rotation = dynamicPhysicalInitializationData->_Rotation;
	transformComponent._Scale = dynamicPhysicalInitializationData->_Scale;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<DynamicPhysicalInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void DynamicPhysicalEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicPhysicalComponentsIndex(_ComponentsIndex);
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