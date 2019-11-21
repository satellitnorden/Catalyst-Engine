//Header file.
#include <Entities/Types/ModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/ModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
ModelEntity::ModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Model;
}

/*
*	Initializes this entity.
*/
void ModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewModelComponentsIndex(this);

	//Copy the data.
	const ModelInitializationData *const RESTRICT model_initialization_data{ static_cast<const ModelInitializationData *const RESTRICT>(data) };
	ModelComponent& component{ ComponentManager::GetModelModelComponents()[_ComponentsIndex] };

	component._Model = model_initialization_data->_Model;
	component._PreviousWorldTransform = model_initialization_data->_Transform;
	component._CurrentWorldTransform = model_initialization_data->_Transform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._Model->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_Transform, &component._WorldSpaceAxisAlignedBoundingBox);
	component._Material = model_initialization_data->_Material;

	//Add the static instance. Assume this model will not move for now.
	if (RenderingSystem::Instance->IsRayTracingSupported())
	{
		RenderingSystem::Instance->GetRayTracingSystem()->AddStaticInstance(TopLevelAccelerationStructureInstanceData(component._PreviousWorldTransform, component._Model->_BottomLevelAccelerationStructure, 0));
	}

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<ModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void ModelEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD Matrix4 *const RESTRICT ModelEntity::GetWorldTransform() NOEXCEPT
{
	return &ComponentManager::GetModelModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT ModelEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetModelModelComponents()[_ComponentsIndex]._Model->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT ModelEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetModelModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the material.
*/
RESTRICTED NO_DISCARD Material *const RESTRICT ModelEntity::GetMaterial() NOEXCEPT
{
	return &ComponentManager::GetModelModelComponents()[_ComponentsIndex]._Material;
}