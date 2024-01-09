//Header file.
#include <Entities/Types/StaticModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/StaticModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Default constructor.
*/
StaticModelEntity::StaticModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::StaticModel;
}

/*
*	Initializes this entity.
*/
void StaticModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this entity.
	_ComponentsIndex = ComponentManager::GetNewStaticModelComponentsIndex(this);

	//Copy the data.
	const StaticModelInitializationData *const RESTRICT model_initialization_data{ static_cast<const StaticModelInitializationData *const RESTRICT>(data) };
	StaticModelComponent& component{ ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex] };

	component._ModelResource = model_initialization_data->_ModelResource;
	component._WorldTransform = model_initialization_data->_WorldTransform;
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._ModelResource->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_WorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	component._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(component._WorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
	component._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(component._WorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);
	component._MaterialResources = model_initialization_data->_MaterialResources;
	component._ModelCollisionConfiguration = model_initialization_data->_ModelCollisionConfiguration;
	component._MeshesVisibleMask = UINT8_MAXIMUM;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<StaticModelInitializationData>(data);

	//Initialize the entity physics.
	if (ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->InitializeEntityPhysics(this);
	}

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnEntityInitialized(this);
}

/*
*	Terminates this entity.
*/
void StaticModelEntity::Terminate() NOEXCEPT
{
	//Terminate the entity physics.
	if (GetModelCollisionConfiguration()._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->TerminateEntityPhysics(this);
	}

	//Tell the ray tracing system.
	RenderingSystem::Instance->GetRayTracingSystem()->OnEntityTerminated(this);

	//Return this entitiy's components index.
	ComponentManager::ReturnStaticModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the initialization data required to duplicate this entity.
*/
RESTRICTED NO_DISCARD EntityInitializationData *const RESTRICT StaticModelEntity::GetDuplicationInitializationData() const NOEXCEPT
{
	//Create the initialization data.
	StaticModelInitializationData *const RESTRICT data{ EntitySystem::Instance->CreateInitializationData<StaticModelInitializationData>() };

	//Set up the initialization data.
	data->_Properties = EntityInitializationData::Property::NONE;
	data->_WorldTransform = *GetWorldTransform();
	data->_ModelResource = GetModelResource();
	data->_MaterialResources = GetMaterialResources();
	data->_ModelCollisionConfiguration = GetModelCollisionConfiguration();

	//Return the initialization data.
	return data;
}

/*
*	Returns the model resources.
*/
NO_DISCARD ResourcePointer<ModelResource> StaticModelEntity::GetModelResource() const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource;
}

/*
*	Sets the model resource.
*/
void StaticModelEntity::SetModelResource(const ResourcePointer<ModelResource> resource) NOEXCEPT
{
	//Set the model resource.
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource = resource;

	//Update the world space axis aligned bounding box.
	UpdateWorldSpaceAxisAlignedBoundingBox();

	//Re-initialize physics.
	if (ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->TerminateEntityPhysics(this);
		PhysicsSystem::Instance->InitializeEntityPhysics(this);
	}
}

/*
*	Returns the material resources.
*/
NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &StaticModelEntity::GetMaterialResources() const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MaterialResources;
}

/*
*	Sets the material resource at the given index.
*/
void StaticModelEntity::SetMaterialResource(const uint8 index, const ResourcePointer<MaterialResource> resource) NOEXCEPT
{
	//Set the material resource.
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MaterialResources[index] = resource;
}

/*
*	Shows the mesh(es) with the specified mask.
*/
void StaticModelEntity::ShowMesh(const uint8 mask) NOEXCEPT
{
	//Set the flag.
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MeshesVisibleMask |= mask;
}

/*
*	Returns whether or not all of the mesh(es) at the specified mask is shown.
*/
NO_DISCARD bool StaticModelEntity::AreAllMeshesShown(const uint8 mask) NOEXCEPT
{
	return (ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MeshesVisibleMask & mask) == mask;
}

/*
*	Returns whether or not any the mesh(es) at the specified mask is shown.
*/
NO_DISCARD bool StaticModelEntity::IsAnyMeshesShown(const uint8 mask) NOEXCEPT
{
	return static_cast<bool>(ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MeshesVisibleMask & mask);
}

/*
*	Hides the mesh(es) with the specified mask.
*/
void StaticModelEntity::HideMesh(const uint8 mask) NOEXCEPT
{
	//Set the flag.
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MeshesVisibleMask &= ~mask;
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT StaticModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldTransform;
}

#if defined(CATALYST_EDITOR)
/*
*	Sets the world transform.
*/
void StaticModelEntity::SetWorldTransform(const WorldTransform &new_world_transform) NOEXCEPT
{
	//Set the new world transform.
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldTransform = new_world_transform;

	//Update the world space axis aligned bounding box.
	UpdateWorldSpaceAxisAlignedBoundingBox();

	//Update the physics entity world transform.
	if (ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->UpdateEntityWorldTransform(this, new_world_transform);
	}

	//Report to the ray tracing system that static models have been updated.
	RenderingSystem::Instance->GetRayTracingSystem()->ReportStaticModelsUpdated();
}
#endif

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3D *const RESTRICT StaticModelEntity::GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const WorldSpaceAxisAlignedBoundingBox3D *const RESTRICT StaticModelEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the level of detail index at the given mesh index.
*/
NO_DISCARD uint64 StaticModelEntity::GetLevelOfDetailindex(const uint64 mesh_index) const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._LevelOfDetailIndices[mesh_index];
}

/*
*	Returns the model collision configuration.
*/
NO_DISCARD const ModelCollisionConfiguration &StaticModelEntity::GetModelCollisionConfiguration() const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration;
}

/*
*	Sets the model collision configuration.
*/
void StaticModelEntity::SetModelCollisionConfiguration(const ModelCollisionConfiguration& value) NOEXCEPT
{
	PhysicsSystem::Instance->TerminateEntityPhysics(this);

	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration = value;

	if (ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelCollisionConfiguration._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->InitializeEntityPhysics(this);
	}
}

/*
*	Updates the world space axis aligned bounding box.
*/
void StaticModelEntity::UpdateWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(*GetModelSpaceAxisAlignedBoundingBox(), GetWorldTransform()->ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(GetWorldTransform()->GetCell(), local_axis_aligned_bounding_box._Minimum);
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(GetWorldTransform()->GetCell(), local_axis_aligned_bounding_box._Maximum);
}