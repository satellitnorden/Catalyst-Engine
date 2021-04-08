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
	component._MeshesVisibleMask = UINT8_MAXIMUM;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<StaticModelInitializationData>(data);

	//Initialize the entity physics.
	PhysicsSystem::Instance->InitializeEntityPhysics(this);
}

/*
*	Terminates this entity.
*/
void StaticModelEntity::Terminate() NOEXCEPT
{
	//Terminate the entity physics.
	PhysicsSystem::Instance->TerminateEntityPhysics(this);

	//Return this entitiy's components index.
	ComponentManager::ReturnStaticModelComponentsIndex(_ComponentsIndex);
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
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource = resource;
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
	ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MaterialResources[index] = resource;
}

/*
*	Shows the mesh(es) with the specified mask.
*/
void StaticModelEntity::ShowMesh(const uint8 mask) NOEXCEPT
{
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
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD WorldTransform *const RESTRICT StaticModelEntity::ModifyWorldTransform() NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldTransform;
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