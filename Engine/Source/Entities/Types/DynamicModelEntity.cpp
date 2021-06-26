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
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._ModelResource->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_InitialWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	component._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(model_initialization_data->_InitialWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
	component._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(model_initialization_data->_InitialWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);
	component._MaterialResources = model_initialization_data->_MaterialResources;
	component._ModelCollisionConfiguration = model_initialization_data->_ModelCollisionConfiguration;
	component._ModelSimulationConfiguration = model_initialization_data->_ModelSimulationConfiguration;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<DynamicModelInitializationData>(data);

	//Initialize the entity physics.
	if (ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->InitializeEntityPhysics(this);
	}
}

/*
*	Terminates this entity.
*/
void DynamicModelEntity::Terminate() NOEXCEPT
{
	//Terminate the entity physics.
	if (GetModelCollisionConfiguration()._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->TerminateEntityPhysics(this);
	}

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
	data->_ModelCollisionConfiguration = GetModelCollisionConfiguration();
	data->_ModelSimulationConfiguration = *GetModelSimulationConfiguration();

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
*	Sets the world transform.
*/
void DynamicModelEntity::SetWorldTransform(const WorldTransform &new_world_transform) NOEXCEPT
{
	//Set the new world transform.
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._CurrentWorldTransform = new_world_transform;

	//Update the world space axis aligned bounding box.
	UpdateWorldSpaceAxisAlignedBoundingBox();

	//Update the physics entity world transform.
	if (ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->UpdateEntityWorldTransform(this, new_world_transform);
	}
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
RESTRICTED NO_DISCARD const WorldSpaceAxisAlignedBoundingBox3D *const RESTRICT DynamicModelEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the model collision configuration.
*/
NO_DISCARD const ModelCollisionConfiguration &DynamicModelEntity::GetModelCollisionConfiguration() const NOEXCEPT
{
	return ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration;
}

/*
*	Sets the model collision configuration.
*/
void DynamicModelEntity::SetModelCollisionConfiguration(const ModelCollisionConfiguration& value) NOEXCEPT
{
	PhysicsSystem::Instance->TerminateEntityPhysics(this);

	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration = value;

	if (ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->InitializeEntityPhysics(this);
	}
}

/*
*	Returns the model simulation configuration.
*/
RESTRICTED NO_DISCARD const ModelSimulationConfiguration *const RESTRICT DynamicModelEntity::GetModelSimulationConfiguration() const NOEXCEPT
{
	return &ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._ModelSimulationConfiguration;
}

/*
*	Updates the world space axis aligned bounding box.
*/
void DynamicModelEntity::UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(*GetModelSpaceAxisAlignedBoundingBox(), GetWorldTransform()->ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(GetWorldTransform()->GetCell(), local_axis_aligned_bounding_box._Minimum);
	ComponentManager::GetDynamicModelDynamicModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(GetWorldTransform()->GetCell(), local_axis_aligned_bounding_box._Maximum);
}