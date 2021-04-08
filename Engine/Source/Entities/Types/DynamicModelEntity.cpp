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

	component._ModelResource = model_initialization_data->_ModelResource;
	component._PreviousWorldTransform = model_initialization_data->_InitialWorldTransform;
	component._CurrentWorldTransform = model_initialization_data->_InitialWorldTransform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._ModelResource->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_InitialWorldTransform.ToLocalMatrix4x4(), &component._WorldSpaceAxisAlignedBoundingBox);
	component._MaterialResources = model_initialization_data->_MaterialResources;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<DynamicModelInitializationData>(data);

	//Initialize the entity physics.
	PhysicsSystem::Instance->InitializeEntityPhysics(this);
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
*	Returns the initialization data required to duplicate this entity.
*/
RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT DynamicModelEntity::GetDuplicationInitializationData() const NOEXCEPT
{
	//Create the initialization data.
	DynamicModelInitializationData* const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<DynamicModelInitializationData>() };

	//Set up the initialization data.
	data->_Properties = EntityInitializationData::Property::NONE;
	data->_InitialWorldTransform = *GetWorldTransform();
	data->_ModelResource = GetModelResource();
	data->_MaterialResources = GetMaterialResources();
	data->_ModelCollisionConfiguration._Type = ModelCollisionType::AXIS_ALIGNED_BOUNDING_BOX;
	data->_SimulatePhysics = false;

	//Return the initialization data.
	return data;
}

/*
*	Returns the model resources.
*/
NO_DISCARD ResourcePointer<ModelResource> DynamicModelEntity::GetModelResource() const NOEXCEPT
{
	return ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelResource;
}

/*
*	Sets the model resource.
*/
void DynamicModelEntity::SetModelResource(const ResourcePointer<ModelResource> resource) NOEXCEPT
{
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelResource = resource;
}

/*
*	Returns the material resources.
*/
NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &DynamicModelEntity::GetMaterialResources() const NOEXCEPT
{
	return ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._MaterialResources;
}

/*
*	Sets the material resource at the given index.
*/
void DynamicModelEntity::SetMaterialResource(const uint8 index, const ResourcePointer<MaterialResource> resource) NOEXCEPT
{
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._MaterialResources[index] = resource;
}

/*
*	Returns the level of detail index at the given mesh index.
*/
NO_DISCARD uint64 DynamicModelEntity::GetLevelOfDetailindex(const uint64 mesh_index) const NOEXCEPT
{
	return ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._LevelOfDetailIndices[mesh_index];
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT DynamicModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the world transform. Assumes the world transform will be modified, and will notify relevant systems.
*/
RESTRICTED NO_DISCARD WorldTransform *const RESTRICT DynamicModelEntity::ModifyWorldTransform() NOEXCEPT
{
	//Remember that the world space axis aligned bounding box needs to be updated.
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags |= DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX;

	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3D *const RESTRICT DynamicModelEntity::GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelResource->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3D *const RESTRICT DynamicModelEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	//Does the world space axis aligned bounding box need to be updated?
	if (TEST_BIT(ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags, DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX))
	{
		//Update the world space axis aligned bounding box.
		RenderingUtilities::TransformAxisAlignedBoundingBox(*GetModelSpaceAxisAlignedBoundingBox(), GetWorldTransform()->ToAbsoluteMatrix4x4(), &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox);
	
		//Clear the update flag.
		CLEAR_BIT(ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._UpdateFlags, DynamicModelComponent::UpdateFlag::WORLD_SPACE_AXIS_ALIGNED_BOUNDING_BOX);
	}

	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}