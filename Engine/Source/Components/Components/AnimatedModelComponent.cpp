//Header file.
#include <Components/Components/AnimatedModelComponent.h>

//Animation.
#include <Animation/AnimationCore.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/AnimationSystem.h>
#include <Systems/RenderingSystem.h>

/*
*	Creates an instance.
*/
void AnimatedModelComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	AnimatedModelInitializationData *const RESTRICT _initialization_data{ static_cast<AnimatedModelInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	AnimatedModelInstanceData &instance_data{ _InstanceData.Back() };

	//Copy data.
	instance_data._AnimatedModelResource = _initialization_data->_AnimatedModelResource;
	//instance_data._MaterialResource = _initialization_data->_MaterialResource;
	instance_data._CurrentAnimationResource = _initialization_data->_InitialAnimationResource;

	//Create the animation data buffers and render data tables.
	const uint8 number_of_framebuffers{ RenderingSystem::Instance->GetNumberOfFramebuffers() };

	instance_data._AnimationDataBuffers.Upsize<false>(number_of_framebuffers);
	instance_data._AnimationDataRenderDataTables.Upsize<false>(number_of_framebuffers);

	for (uint8 i{ 0 }; i < number_of_framebuffers; ++i)
	{
		RenderingSystem::Instance->CreateBuffer(sizeof(Matrix4x4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible, &instance_data._AnimationDataBuffers[i]);

		StaticArray<Matrix4x4, AnimationConstants::MAXIMUM_BONE_TRANSFORMS> initial_bone_transforms;

		const void *const RESTRICT dataChunks[]{ initial_bone_transforms.Data() };
		const uint64 dataSizes[]{ sizeof(Matrix4x4) * AnimationConstants::MAXIMUM_BONE_TRANSFORMS };

		RenderingSystem::Instance->UploadDataToBuffer(dataChunks, dataSizes, 1, &instance_data._AnimationDataBuffers[i]);

		RenderingSystem::Instance->CreateRenderDataTable(AnimationSystem::Instance->GetAnimationDataRenderDataTableLayout(), &instance_data._AnimationDataRenderDataTables[i]);
		RenderingSystem::Instance->BindUniformBufferToRenderDataTable(0, 0, &instance_data._AnimationDataRenderDataTables[i], instance_data._AnimationDataBuffers[i]);
	}
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void AnimatedModelComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Animated model component needs a world transform component!");

	//Cache the instance data.
	const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };
	AnimatedModelInstanceData &animated_model_instance_data{ InstanceData(entity) };

	//Calculate the world space axis aligned bounding box.
	AxisAlignedBoundingBox3D local_axis_aligned_bounding_box;
	RenderingUtilities::TransformAxisAlignedBoundingBox(animated_model_instance_data._AnimatedModelResource->_ModelSpaceAxisAlignedBoundingBox, world_transform_instance_data._CurrentWorldTransform.ToLocalMatrix4x4(), &local_axis_aligned_bounding_box);
	animated_model_instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Minimum);
	animated_model_instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum = WorldPosition(world_transform_instance_data._CurrentWorldTransform.GetCell(), local_axis_aligned_bounding_box._Maximum);
}

/*
*	Destroys an instance.
*/
void AnimatedModelComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	AnimatedModelInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the buffers and render data tables.
	const uint8 number_of_framebuffers{ RenderingSystem::Instance->GetNumberOfFramebuffers() };

	for (uint8 i{ 0 }; i < number_of_framebuffers; ++i)
	{
		RenderingSystem::Instance->DestroyBuffer(&instance_data._AnimationDataBuffers[i]);
		RenderingSystem::Instance->DestroyRenderDataTable(&instance_data._AnimationDataRenderDataTables[i]);
	}

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 AnimatedModelComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void AnimatedModelComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 64;
}

/*
*	Updates this component.
*/
void AnimatedModelComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	
}