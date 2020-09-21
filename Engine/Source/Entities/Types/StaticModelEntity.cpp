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
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._ModelResource->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_WorldTransform.ToLocalMatrix4x4(), &component._WorldSpaceAxisAlignedBoundingBox);
	component._MaterialResources = model_initialization_data->_MaterialResources;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<StaticModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void StaticModelEntity::Terminate() NOEXCEPT
{
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
*	Returns the material resources.
*/
NO_DISCARD const StaticArray<ResourcePointer<MaterialResource>, RenderingConstants::MAXIMUM_NUMBER_OF_MESHES_PER_MODEL> &StaticModelEntity::GetMaterialResources() const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._MaterialResources;
}

/*
*	Returns the level of detail index at the given mesh index.
*/
NO_DISCARD uint64 StaticModelEntity::GetLevelOfDetailindex(const uint64 mesh_index) const NOEXCEPT
{
	return ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._LevelOfDetailIndices[mesh_index];
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD const WorldTransform *const RESTRICT StaticModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT StaticModelEntity::GetModelSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT StaticModelEntity::GetWorldSpaceAxisAlignedBoundingBox() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}