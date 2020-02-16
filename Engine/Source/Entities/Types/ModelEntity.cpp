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
#include <Systems/PhysicsSystem.h>

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
	component._MaterialIndices = std::move(model_initialization_data->_MaterialIndices);

	//Add the static instance. Assume this model will not move for now.
	if (RenderingSystem::Instance->IsRayTracingSupported())
	{
		for (const Mesh &mesh : component._Model->_Meshes)
		{
			RenderingSystem::Instance->GetRayTracingSystem()->AddStaticInstance(TopLevelAccelerationStructureInstanceData(component._PreviousWorldTransform, mesh._BottomLevelAccelerationStructure, 0));
		}
		
	}

	//Register the model collision data, if there is one.
	if (model_initialization_data->_ModelCollisionData._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->RegisterModelCollisionData(_ComponentsIndex, model_initialization_data->_ModelCollisionData);
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
*	Enables highlight on this model entity.
*/
void ModelEntity::EnableHighlight(const Vector3<float>& color, const float strength) NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->EnableHighlight(this, color, strength);
}

/*
*	Returns if this model entity is highlighted or not.
*/
NO_DISCARD bool ModelEntity::IsHighlighted() const NOEXCEPT
{
	for (const HighlightedModel &highlighted_model : *RenderingSystem::Instance->GetModelSystem()->GetHighlightedModels())
	{
		if (_ComponentsIndex == highlighted_model._ComponentsIndex)
		{
			return true;
		}
	}

	return false;
}

/*
*	Sets the highlight color on this model entity.
*/
void ModelEntity::SetHighlightColor(const Vector3<float> &color) NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->SetHighlightColor(this, color);
}

/*
*	Disables highlight on this model entity.
*/
void ModelEntity::DisableHighlight() NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->DisableHighlight(this);
}