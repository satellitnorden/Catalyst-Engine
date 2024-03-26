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

DEFINE_COMPONENT(AnimatedModelComponent, AnimatedModelInitializationData, AnimatedModelInstanceData);

/*
*	Post-initializes this component.
*/
void AnimatedModelComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void AnimatedModelComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool AnimatedModelComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void AnimatedModelComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

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
*	Runs before the given update phase.
*/
void AnimatedModelComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

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
	/*
	PROFILING_SCOPE(StaticModelComponent::Update);

	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const WorldTransform  camera_world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };
			const Vector3<int32> camera_cell{ camera_world_transform.GetCell() };
			const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix() };
			const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_index }; instance_index < end_index; ++instance_index)
			{
				//Cache the instance data.
				StaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

				//Update the world transform is this static model instance is physics-simulated.
				if (instance_data._PhysicsActorHandle && instance_data._ModelSimulationConfiguration._SimulatePhysics)
				{
					WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(InstanceToEntity(instance_index)) };
					PhysicsSystem::Instance->GetActorWorldTransform(instance_data._PhysicsActorHandle, &world_transform_instance_data._CurrentWorldTransform);
				}

				//Do culling.
				bool visible{ false };

				{
					//Reset the visibility flags.
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

					//Do camera culling.
					{
						//Do frustum culling.
						if (!Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA);
						}

						else
						{
							visible = true;
						}
					}

					//Do shadow map culling.
					for (uint32 shadow_map_data_index{ 0 }; shadow_map_data_index < RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData(); ++shadow_map_data_index)
					{
						const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(shadow_map_data_index) };

						//Do frustum culling.
						if (!Culling::IsWithinFrustum(instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), shadow_map_data._Frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::SHADOW_MAP_START << shadow_map_data_index);
						}

						else
						{
							visible = true;
						}
					}
				}

				//Do level of detail.
				if (visible)
				{
					for (uint64 mesh_index{ 0 }, size{ instance_data._ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
					{
						//If the mesh used only has one level of detail, skip it.
						if (instance_data._ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() == 1)
						{
							instance_data._LevelOfDetailIndices[mesh_index] = 0;

							continue;
						}

						//Retrieve the relative axis aligned bounding box.
						const AxisAlignedBoundingBox3D relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_world_transform.GetCell()) };

						//Calculate the minimum/maximum screen coordinates from the corners of the relative axis aligned bounding box.
						Vector2<float32> minimum_screen_coordinate{ FLOAT32_MAXIMUM, FLOAT32_MAXIMUM };
						Vector2<float32> maximum_screen_coordinate{ -FLOAT32_MAXIMUM, -FLOAT32_MAXIMUM };

						const StaticArray<Vector3<float32>, 8> corners
						{
							Vector3<float32>(relative_axis_aligned_bounding_box._Minimum._X, relative_axis_aligned_bounding_box._Minimum._Y, relative_axis_aligned_bounding_box._Minimum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Minimum._X, relative_axis_aligned_bounding_box._Minimum._Y, relative_axis_aligned_bounding_box._Maximum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Minimum._X, relative_axis_aligned_bounding_box._Maximum._Y, relative_axis_aligned_bounding_box._Minimum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Minimum._X, relative_axis_aligned_bounding_box._Maximum._Y, relative_axis_aligned_bounding_box._Maximum._Z),

							Vector3<float32>(relative_axis_aligned_bounding_box._Maximum._X, relative_axis_aligned_bounding_box._Minimum._Y, relative_axis_aligned_bounding_box._Minimum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Maximum._X, relative_axis_aligned_bounding_box._Minimum._Y, relative_axis_aligned_bounding_box._Maximum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Maximum._X, relative_axis_aligned_bounding_box._Maximum._Y, relative_axis_aligned_bounding_box._Minimum._Z),
							Vector3<float32>(relative_axis_aligned_bounding_box._Maximum._X, relative_axis_aligned_bounding_box._Maximum._Y, relative_axis_aligned_bounding_box._Maximum._Z)
						};

						for (uint8 corner_index{ 0 }; corner_index < 8; ++corner_index)
						{
							Vector4<float32> screen_space_position{ *camera_world_to_clip_matrix * Vector4<float32>(corners[corner_index], 1.0f) };
							const float32 screen_space_position_reciprocal{ 1.0f / screen_space_position._W };

							Vector2<float32> screen_coordinate{ screen_space_position._X * screen_space_position_reciprocal, screen_space_position._Y * screen_space_position_reciprocal };

							screen_coordinate._X = screen_coordinate._X * 0.5f + 0.5f;
							screen_coordinate._Y = screen_coordinate._Y * 0.5f + 0.5f;

							minimum_screen_coordinate = Vector2<float32>::Minimum(minimum_screen_coordinate, screen_coordinate);
							maximum_screen_coordinate = Vector2<float32>::Maximum(maximum_screen_coordinate, screen_coordinate);
						}

						//Calculate the screen coverage.
						const float32 screen_coverage{ CatalystBaseMath::Minimum<float32>((maximum_screen_coordinate._X - minimum_screen_coordinate._X) * (maximum_screen_coordinate._Y - minimum_screen_coordinate._Y), 1.0f) };

						//Calculate the level of detail index.
						instance_data._LevelOfDetailIndices[mesh_index] = static_cast<uint32>((1.0f - screen_coverage) * static_cast<float32>(instance_data._ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() - 1));
					}
				}
			}

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
	*/
}

/*
*	Runs after the given update phase.
*/
void AnimatedModelComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}