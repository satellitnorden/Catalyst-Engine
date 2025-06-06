//Header file.
#include <Components/Components/GrassComponent.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Rendering.
#include <Rendering/Native/Culling.h>
#include <Rendering/Native/GrassCore.h>

//Systems.
#include <Systems/RenderingSystem.h>

/*
*	Updates this component.
*/
void GrassComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
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
				GrassInstanceData &instance_data{ _InstanceData[instance_index] };

				//Cache the camera relative axis aligned bounding box.
				const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

				//Do culling.
				{
					//Reset the visibility flags.
					instance_data._VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

					//Do distance culling.
					{
						const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
						const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

						if (distance_squared >= (instance_data._FadeOutDistance * instance_data._FadeOutDistance))
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

				//Do level of detail.
				if (TEST_BIT(instance_data._VisibilityFlags, VisibilityFlags::CAMERA))
				{
					//Calculate the distance.
					const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
					const float32 distance{ Vector3<float32>::Length(camera_local_position - closest_position) };

					//Calculate the level of detail.
					instance_data._LevelOfDetail = BaseMath::Minimum<uint8>(static_cast<uint8>(distance / GrassConstants::LEVEL_OF_DETAIL_DISTANCE_FACTOR), GrassConstants::HIGHEST_LEVEL_OF_DETAIL);
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
void GrassComponent::CreateInstance(Entity *const RESTRICT entity, GrassInitializationData *const RESTRICT initialization_data, GrassInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy the data.
	ASSERT(!initialization_data->_GrassInstances.Empty(), "Trying to add grass component without any instances!");

	//Set the world grid cell.
	instance_data->_WorldGridCell = initialization_data->_WorldGridCell;

	//Calculate the axis aligned bounding box.
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Minimum.SetCell(instance_data->_WorldGridCell);
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Maximum.SetCell(instance_data->_WorldGridCell);

	Vector3<float32> minimum{ FLOAT32_MAXIMUM };
	Vector3<float32> maximum{ -FLOAT32_MAXIMUM };

	for (const GrassInstance &grass_instance : initialization_data->_GrassInstances)
	{
		minimum = BaseMath::Minimum(minimum, grass_instance._Position);
		maximum = BaseMath::Maximum(maximum, grass_instance._Position);
	}

	instance_data->_WorldSpaceAxisAlignedBoundingBox._Minimum.SetLocalPosition(minimum);
	instance_data->_WorldSpaceAxisAlignedBoundingBox._Maximum.SetLocalPosition(maximum);

	//Pack the grass instances.
	struct PackedGrassInstance final
	{
		uint32 _InstanceData1;
		uint32 _InstanceData2;
	};

	DynamicArray<PackedGrassInstance> packed_instances;
	packed_instances.Reserve(initialization_data->_GrassInstances.Size());

	for (const GrassInstance &grass_instance : initialization_data->_GrassInstances)
	{
		packed_instances.Emplace();
		PackedGrassInstance &new_packed_grass_instance{ packed_instances.Back() };

		const Vector3<float32> normalized_position
		{
			BaseMath::Scale(grass_instance._Position._X, minimum._X, maximum._X, 0.0f, 1.0f),
			BaseMath::Scale(grass_instance._Position._Y, minimum._Y, maximum._Y, 0.0f, 1.0f),
			BaseMath::Scale(grass_instance._Position._Z, minimum._Z, maximum._Z, 0.0f, 1.0f)
		};

		const Vector3<uint16> quantized_position
		{
			static_cast<uint16>(normalized_position._X * static_cast<float32>(UINT16_MAXIMUM)),
			static_cast<uint16>(normalized_position._Y * static_cast<float32>(UINT16_MAXIMUM)),
			static_cast<uint16>(normalized_position._Z * static_cast<float32>(UINT16_MAXIMUM))
		};

		const float32 normalized_rotation{ BaseMath::Scale(grass_instance._Rotation, -BaseMathConstants::PI, BaseMathConstants::PI, 0.0f, 1.0f) };

		const uint16 quantized_rotation{ static_cast<uint16>(normalized_rotation * static_cast<float32>(UINT16_MAXIMUM)) };

		new_packed_grass_instance._InstanceData1 = (static_cast<uint32>(quantized_position._X)) | (static_cast<uint32>(quantized_position._Y) << 16);
		new_packed_grass_instance._InstanceData2 = (static_cast<uint32>(quantized_position._Z)) | (static_cast<uint32>(quantized_rotation) << 16);
	}

	const void *RESTRICT positions_data[]{ packed_instances.Data() };
	const uint64 positions_data_sizes[]{ sizeof(PackedGrassInstance) * packed_instances.Size() };
	RenderingSystem::Instance->CreateBuffer(positions_data_sizes[0], BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &instance_data->_InstanceBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(positions_data, positions_data_sizes, 1, &instance_data->_InstanceBuffer);

	instance_data->_NumberOfInstances = static_cast<uint32>(packed_instances.Size());

	//Copy other properties.
	instance_data->_Material = initialization_data->_Material;
	instance_data->_Thickness = initialization_data->_Thickness;
	instance_data->_Height = initialization_data->_Height;
	instance_data->_Tilt = initialization_data->_Tilt;
	instance_data->_Bend = initialization_data->_Bend;
	instance_data->_FadeOutDistance = initialization_data->_FadeOutDistance;
}

/*
*	Destroys an instance.
*/
void GrassComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	GrassInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the instance buffer.
	RenderingSystem::Instance->DestroyBuffer(&instance_data._InstanceBuffer);

	//Remove the instance.
	RemoveInstance(entity);
}