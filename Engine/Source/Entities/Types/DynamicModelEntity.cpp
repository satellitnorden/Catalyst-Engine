//Header file.
#include <Entities/Types/DynamicModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
DynamicModelEntity::DynamicModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::DynamicModel;
}

/*
*	Initializes this entity.
*/
void DynamicModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewDynamicModelComponentsIndex(this);

	//Copy the data.
	const DynamicModelInitializationData *const RESTRICT dynamicModelInitializationData{ static_cast<const DynamicModelInitializationData *const RESTRICT>(data) };
	DynamicModelComponent& dynamicModelComponent{ ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex] };

	dynamicModelComponent._Model = dynamicModelInitializationData->_Model;
	dynamicModelComponent._PreviousWorldTransform = dynamicModelInitializationData->_Transform;
	dynamicModelComponent._CurrentWorldTransform = dynamicModelInitializationData->_Transform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(dynamicModelComponent._Model->_ModelSpaceAxisAlignedBoundingBox, dynamicModelInitializationData->_Transform, &dynamicModelComponent._WorldSpaceAxisAlignedBoundingBox);
	dynamicModelComponent._Material = dynamicModelInitializationData->_Material;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<DynamicModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void DynamicModelEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD Matrix4 *const RESTRICT DynamicModelEntity::GetWorldTransform() NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT DynamicModelEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._Model->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT DynamicModelEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the material.
*/
RESTRICTED NO_DISCARD Material *const RESTRICT DynamicModelEntity::GetMaterial() NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._Material;
}