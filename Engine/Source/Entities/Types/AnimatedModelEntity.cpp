//Header file.
#include <Entities/Types/AnimatedModelEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/AnimatedModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/EntityCreationSystem.h>

/*
*	Default constructor.
*/
AnimatedModelEntity::AnimatedModelEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::AnimatedModel;
}

/*
*	Initializes this entity.
*/
void AnimatedModelEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewAnimatedModelComponentsIndex(this);

	//Copy the data.
	const AnimatedModelInitializationData *const RESTRICT animatedModelInitializationData{ static_cast<const AnimatedModelInitializationData *const RESTRICT>(data) };
	AnimatedModelComponent& animatedModelComponent{ ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex] };

	animatedModelComponent._Model = animatedModelInitializationData->_Model;
	animatedModelComponent._PreviousWorldTransform = animatedModelInitializationData->_Transform;
	animatedModelComponent._CurrentWorldTransform = animatedModelInitializationData->_Transform;
	RenderingUtilities::TransformAxisAlignedBoundingBox(animatedModelComponent._Model->_ModelSpaceAxisAlignedBoundingBox, animatedModelInitializationData->_Transform, &animatedModelComponent._WorldSpaceAxisAlignedBoundingBox);
	animatedModelComponent._Material = animatedModelInitializationData->_Material;

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<AnimatedModelInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void AnimatedModelEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnAnimatedModelComponentsIndex(_ComponentsIndex);
}

/*
*	Returns the world transform.
*/
RESTRICTED NO_DISCARD Matrix4 *const RESTRICT AnimatedModelEntity::GetWorldTransform() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT AnimatedModelEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._Model->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox *const RESTRICT AnimatedModelEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._WorldSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the material.
*/
RESTRICTED NO_DISCARD Material *const RESTRICT AnimatedModelEntity::GetMaterial() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._Material;
}