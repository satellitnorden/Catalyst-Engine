//Header file.
#include <Entities/Types/DynamicModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/DynamicModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntitySystem.h>
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
	component._MaterialIndexCollection = model_initialization_data->_MaterialIndexCollection;

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

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<DynamicModelInitializationData>(data);
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
RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT DynamicModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the world transform. Assumes the world transform will be modified, and will notify relevant systems.
*/
RESTRICTED NO_DISCARD Matrix4x4 *const RESTRICT DynamicModelEntity::ModifyWorldTransform() NOEXCEPT
{
	//Remember that the world space axis aligned bounding box needs to be updated.
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags |= DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX;

	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT DynamicModelEntity::GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._Model->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT DynamicModelEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	//Does the world space axis aligned bounding box need to be updated?
	if (TEST_BIT(ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags, DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX))
	{
		//Update the world space axis aligned bounding box.
		RenderingUtilities::TransformAxisAlignedBoundingBox(*GetModelSpaceAxisAlignedBoundingBox(), *GetWorldTransform(), &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox);
	
		//Clear the update flag.
		CLEAR_BIT(ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags, DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX);
	}

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