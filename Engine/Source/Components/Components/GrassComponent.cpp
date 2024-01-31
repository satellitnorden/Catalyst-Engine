//Header file.
#include <Components/Components/GrassComponent.h>

//Math.
#include <Math/Core/CatalystRandomMath.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>
#include <Rendering/Native/GrassCore.h>

//Systems.
#include <Systems/RenderingSystem.h>

DEFINE_COMPONENT(GrassComponent, GrassSharedData, GrassInitializationData, GrassInstanceData);

/*
*	Initializes this component.
*/
void GrassComponent::Initialize() NOEXCEPT
{

}

/*
*	Post-initializes this component.
*/
void GrassComponent::PostInitialize() NOEXCEPT
{

}

NO_DISCARD bool GrassComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void GrassComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void GrassComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	GrassInitializationData *const RESTRICT _initialization_data{ static_cast<GrassInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	GrassInstanceData &instance_data{ _InstanceData.Back() };

	//Copy the data.
	ASSERT(!_initialization_data->_WorldPositions.Empty(), "Trying to add grass entity without any positions!");

	//Calculate the cell.
	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldPosition &world_position : _initialization_data->_WorldPositions)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_position.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(_initialization_data->_WorldPositions.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		instance_data._Cell[i] = CatalystBaseMath::Round<int32>(average_cell[i]);
	}

	//Calculate the axis aligned bounding box.
	instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum.SetCell(instance_data._Cell);
	instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum.SetCell(instance_data._Cell);

	Vector3<float32> minimum{ FLOAT32_MAXIMUM };
	Vector3<float32> maximum{ -FLOAT32_MAXIMUM };

	for (const WorldPosition &world_position : _initialization_data->_WorldPositions)
	{
		const Vector3<float32> relative_position{ world_position.GetRelativePosition(instance_data._Cell) };

		minimum = Vector3<float32>::Minimum(minimum, relative_position);
		maximum = Vector3<float32>::Maximum(maximum, relative_position);
	}

	instance_data._WorldSpaceAxisAlignedBoundingBox._Minimum.SetLocalPosition(minimum);
	instance_data._WorldSpaceAxisAlignedBoundingBox._Maximum.SetLocalPosition(maximum);

	struct GrassInstance final
	{
		Vector3<float32> _Position;
		uint32 _Seed;
	};

	DynamicArray<GrassInstance> instances;
	instances.Reserve(_initialization_data->_WorldPositions.Size());

	for (const WorldPosition &world_position : _initialization_data->_WorldPositions)
	{
		instances.Emplace();
		GrassInstance &new_grass_instance{ instances.Back() };

		new_grass_instance._Position = world_position.GetRelativePosition(instance_data._Cell);
		new_grass_instance._Seed = CatalystRandomMath::RandomIntegerInRange<uint32>(0, UINT32_MAXIMUM);
	}

	const void *RESTRICT positions_data[]{ instances.Data() };
	const uint64 positions_data_sizes[]{ sizeof(GrassInstance) * instances.Size() };
	RenderingSystem::Instance->CreateBuffer(positions_data_sizes[0], BufferUsage::VertexBuffer, MemoryProperty::DeviceLocal, &instance_data._InstanceBuffer);
	RenderingSystem::Instance->UploadDataToBuffer(positions_data, positions_data_sizes, 1, &instance_data._InstanceBuffer);

	instance_data._NumberOfInstances = static_cast<uint32>(instances.Size());

	//Copy other properties.
	instance_data._MaterialResource = _initialization_data->_MaterialResource;
	instance_data._Thickness = _initialization_data->_Thickness;
	instance_data._Height = _initialization_data->_Height;
	instance_data._Tilt = _initialization_data->_Tilt;
	instance_data._Bend = _initialization_data->_Bend;
	instance_data._FadeOutDistance = _initialization_data->_FadeOutDistance;

	//Free the initialization data.
	FreeInitializationData(_initialization_data);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void GrassComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{

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

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 GrassComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void GrassComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 128;
}

/*
*	Runs before the given update phase.
*/
void GrassComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void GrassComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("GrassComponent::Update");

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
					instance_data._LevelOfDetail = CatalystBaseMath::Minimum<uint8>(static_cast<uint8>(distance / GrassConstants::LEVEL_OF_DETAIL_DISTANCE_FACTOR), GrassConstants::HIGHEST_LEVEL_OF_DETAIL);
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
*	Runs after the given update phase.
*/
void GrassComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}