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
#include <Systems/PhysicsSystem.h>

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
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewDynamicModelComponentsIndex(this);

	//Copy the data.
	const DynamicModelInitializationData *const RESTRICT model_initialization_data{ static_cast<const DynamicModelInitializationData *const RESTRICT>(data) };
	DynamicModelComponent& component{ ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex] };

	component._Model = model_initialization_data->_Model;
	component._PreviousWorldTransform = model_initialization_data->_Transform;
	component._CurrentWorldTransform = model_initialization_data->_Transform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._Model->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_Transform, &component._WorldSpaceAxisAlignedBoundingBox);
	component._MaterialIndices = std::move(model_initialization_data->_MaterialIndices);

	//Register the model collision data, if there is one.
	if (model_initialization_data->_ModelCollisionData._Type != ModelCollisionType::NONE)
	{
		//PhysicsSystem::Instance->GetModelPhysicsSystem()->RegisterModelCollisionData(_ComponentsIndex, model_initialization_data->_ModelCollisionData);
	}

	//Register the model physics simulation data, if physics should be simulated.
	if (model_initialization_data->_SimulatePhysics)
	{
		PhysicsSystem::Instance->GetModelPhysicsSystem()->RegisterModelPhysicsSimulationData(_ComponentsIndex, model_initialization_data->_ModelPhysicsSimulationData);
	}

	//Notify the ray tracing system that this dynamic model was initialized.
	RenderingSystem::Instance->GetRayTracingSystem()->NofityDynamicModelInitialized();

	//Upsize the level of detail indices.
	component._LevelOfDetailIndices.Upsize<false>(component._Model->_Meshes.Size());

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<DynamicModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void DynamicModelEntity::Terminate() NOEXCEPT
{
	//Notify the ray tracing system that this dynamic model was terminated.
	RenderingSystem::Instance->GetRayTracingSystem()->NofityDynamicModelTerminated();

	//Return this entitiy's components index.
	ComponentManager::ReturnDynamicModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT DynamicModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the world transform. Assumes the world transform will be modified, and will notify relevant systems.
*/
RESTRICTED NO_DISCARD Matrix4x4 *const RESTRICT DynamicModelEntity::ModifyWorldTransform() NOEXCEPT
{
	//Notify the ray tracing system that this dynamic model was modified.
	RenderingSystem::Instance->GetRayTracingSystem()->NofityDynamicModelModified();

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
*	Enables highlight on this model entity.
*/
void DynamicModelEntity::EnableHighlight(const Vector3<float>& color, const float strength) NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->EnableHighlight(this, color, strength);
}

/*
*	Returns if this model entity is highlighted or not.
*/
NO_DISCARD bool DynamicModelEntity::IsHighlighted() const NOEXCEPT
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
void DynamicModelEntity::SetHighlightColor(const Vector3<float> &color) NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->SetHighlightColor(this, color);
}

/*
*	Sets the highlight strength on this model entity.
*/
void DynamicModelEntity::SethighlightStrength(const float32 strength) NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->SetHighlightStrength(this, strength);
}

/*
*	Disables highlight on this model entity.
*/
void DynamicModelEntity::DisableHighlight() NOEXCEPT
{
	RenderingSystem::Instance->GetModelSystem()->DisableHighlight(this);
}