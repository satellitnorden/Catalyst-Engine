//Header file.
#include <Entities/Types/AnimatedModelEntity.h>

//Animation.
#include <Animation/AnimationCore.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/AnimatedModelInitializationData.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/EntitySystem.h>

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
	const AnimatedModelInitializationData *const RESTRICT animated_model_initialization_data{ static_cast<const AnimatedModelInitializationData *const RESTRICT>(data) };
	AnimatedModelComponent& animated_model_component{ ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex] };

	animated_model_component._AnimatedModelResource = animated_model_initialization_data->_AnimatedModelResource;
	animated_model_component._Material = animated_model_initialization_data->_Material;
	animated_model_component._CurrentAnimationResource = animated_model_initialization_data->_InitialAnimationResource;
	RenderingUtilities::TransformAxisAlignedBoundingBox(animated_model_component._AnimatedModelResource->_ModelSpaceAxisAlignedBoundingBox, animated_model_initialization_data->_Transform, &animated_model_component._WorldSpaceAxisAlignedBoundingBox);
	animated_model_component._PreviousWorldTransform = animated_model_initialization_data->_Transform;
	animated_model_component._CurrentWorldTransform = animated_model_initialization_data->_Transform;

	//Create the animation data buffers and render data tables.
	const uint8 number_of_framebuffers{ RenderingSystem::Instance->GetNumberOfFramebuffers() };

	animated_model_component._AnimationDataBuffers.Upsize<false>(number_of_framebuffers);
	animated_model_component._AnimationDataRenderDataTables.Upsize<false>(number_of_framebuffers);

	for (uint8 i{ 0 }; i < number_of_framebuffers; ++i)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(Matrix4x4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &animated_model_component._AnimationDataBuffers[i]);

		StaticArray<Matrix4x4, AnimationConstants::MAXIMUM_BONE_TRANSFORMS> initial_bone_transforms;

		const void *const RESTRICT dataChunks[]{ initial_bone_transforms.Data() };
		const uint64 dataSizes[]{ sizeof(Matrix4x4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS };

		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &animated_model_component._AnimationDataBuffers[i]);

		RenderingSystem::Instance->CreateRenderDataTable(AnimationSystem::Instance->GetAnimationDataRenderDataTableLayout(), &animated_model_component._AnimationDataRenderDataTables[i]);
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &animated_model_component._AnimationDataRenderDataTables[i], animated_model_component._AnimationDataBuffers[i]);

	}

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<AnimatedModelInitializationData>(data);
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
RESTRICTED NO_DISCARD Matrix4x4 *const RESTRICT AnimatedModelEntity::GetWorldTransform() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._CurrentWorldTransform;
}

/*
*	Returns the model space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT AnimatedModelEntity::GetModelSpaceAxisAlignedBoundingBox() NOEXCEPT
{
	return &ComponentManager::GetAnimatedModelAnimatedModelComponents()[_ComponentsIndex]._AnimatedModelResource->_ModelSpaceAxisAlignedBoundingBox;
}

/*
*	Returns the world space axis aligned bounding box.
*/
RESTRICTED NO_DISCARD const AxisAlignedBoundingBox3 *const RESTRICT AnimatedModelEntity::GetWorldSpaceAxisAlignedBoundingBox() NOEXCEPT
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