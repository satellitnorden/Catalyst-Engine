//Header file.
#include <Components/Components/InstancedImpostorComponent.h>

//Rendering.
#include <Rendering/Native/Culling.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Updates this component.
*/
void InstancedImpostorComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
	switch (update_phase)
	{
		case UpdatePhase::PRE_RENDER:
		{
			//Cache data that will be used.
			const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
			const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
			const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				InstancedImpostorInstanceData &instance_data{ _InstanceData[instance_index] };

				//Do culling.
				{
					//Reset the visibility flags.
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

					//Cache the camera relative axis aligned bounding box.
					const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

					//Do distance culling.
					{
						const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
						const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

						if (distance_squared >= (instance_data._EndFadeOutDistance * instance_data._EndFadeOutDistance))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA);
						}
					}

					//Do frustum culling.
					if (TEST_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
					{
						if (!Culling::IsWithinFrustum(camera_relative_axis_aligned_bounding_box, *frustum))
						{
							CLEAR_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA);
						}
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
}

/*
*	Creates an instance.
*/
void InstancedImpostorComponent::CreateInstance(Entity *const RESTRICT entity, InstancedImpostorInitializationData *const RESTRICT initialization_data, InstancedImpostorInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Set up the instance data.
	instance_data->_Material = initialization_data->_Material;
	instance_data->_Dimensions = initialization_data->_Dimensions;
	instance_data->_StartFadeInDistance = initialization_data->_StartFadeInDistance;
	instance_data->_EndFadeInDistance = initialization_data->_EndFadeInDistance;
	instance_data->_StartFadeOutDistance = initialization_data->_StartFadeOutDistance;
	instance_data->_EndFadeOutDistance = initialization_data->_EndFadeOutDistance;

	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldPosition &world_position : initialization_data->_WorldPositions)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_position.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(initialization_data->_WorldPositions.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		instance_data->_Cell[i] = BaseMath::Round<int32>(average_cell[i]);
	}

	//Calculate the axis aligned bounding box.
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Minimum.SetCell(instance_data->_Cell);
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Maximum.SetCell(instance_data->_Cell);

	Vector3<float32> minimum{ FLOAT32_MAXIMUM };
	Vector3<float32> maximum{ -FLOAT32_MAXIMUM };

	for (const WorldPosition &world_position : initialization_data->_WorldPositions)
	{
		const Vector3<float32> relative_position{ world_position.GetRelativePosition(instance_data->_Cell) };

		minimum = BaseMath::Minimum(minimum, relative_position);
		maximum = BaseMath::Maximum(maximum, relative_position);
	}

	minimum -= Vector3<float32>(instance_data->_Dimensions._X * 0.5f, 0.0f, instance_data->_Dimensions._X * 0.5f);
	maximum += Vector3<float32>(instance_data->_Dimensions._X * 0.5f, instance_data->_Dimensions._Y, instance_data->_Dimensions._X * 0.5f);

	instance_data->_WorldSpaceAxisAlignedBoundingBox._Minimum.SetLocalPosition(minimum);
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Maximum.SetLocalPosition(maximum);

	DynamicArray<Vector3<float32>> transformations;
	transformations.Reserve(initialization_data->_WorldPositions.Size());

	for (const WorldPosition &world_position : initialization_data->_WorldPositions)
	{
		transformations.Emplace(world_position.GetRelativePosition(instance_data->_Cell));
	}

	const void *RESTRICT transformation_data[]{ transformations.Data() };
	const uint64 transformation_data_sizes[]{ sizeof(Vector3<float32>) * transformations.Size() };
	RenderingSystem::Instance->CreateBuffer(transformation_data_sizes[0], BufferUsage::IndexBuffer | BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &instance_data->_TransformationsBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(transformation_data, transformation_data_sizes, 1, &instance_data->_TransformationsBuffer);

	instance_data->_NumberOfTransformations = static_cast<uint32>(transformations.Size());
}

/*
*	Destroys an instance.
*/
void InstancedImpostorComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	InstancedImpostorInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the transformations buffer.
	RenderingSystem::Instance->DestroyBuffer(&instance_data._TransformationsBuffer);

	//Remove the instance.
	RemoveInstance(entity);
}

#if !defined(CATALYST_CONFIGURATION_FINAL)
/*
*	Gathers statistics.
*/
void InstancedImpostorComponent::Statistics(ComponentStatistics *const RESTRICT statistics) NOEXCEPT
{
	statistics->_CPUMemoryUsage = sizeof(InstancedImpostorInstanceData) * NumberOfInstances();

	for (const InstancedImpostorInstanceData &instance_data : _InstanceData)
	{
		statistics->_GPUMemoryUsage += sizeof(Vector3<float32>) * instance_data._NumberOfTransformations;
	}
}
#endif