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
	component._WorldTransform = model_initialization_data->_Transform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(component._ModelResource->_ModelSpaceAxisAlignedBoundingBox, model_initialization_data->_Transform, &component._WorldSpaceAxisAlignedBoundingBox);
	component._MaterialIndices = std::move(model_initialization_data->_MaterialIndices);

	//Register the model collision data, if there is one.
	if (model_initialization_data->_ModelCollisionData._Type != ModelCollisionType::NONE)
	{
		PhysicsSystem::Instance->GetModelPhysicsSystem()->RegisterStaticModelCollisionData(_ComponentsIndex, model_initialization_data->_ModelCollisionData);
	}

	//Upsize the level of detail indices.
	component._LevelOfDetailIndices.Upsize<false>(component._ModelResource->_Meshes.Size());

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
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD const Matrix4x4 *const RESTRICT StaticModelEntity::GetWorldTransform() const NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT StaticModelEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._ModelResource->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT StaticModelEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetStaticModelStaticModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}