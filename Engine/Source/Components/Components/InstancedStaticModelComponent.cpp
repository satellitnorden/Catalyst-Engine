//Header file.
#include <Components/Components/InstancedStaticModelComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>
#include <Rendering/Native/RenderingUtilities.h>

DEFINE_COMPONENT(InstancedStaticModelComponent, InstancedStaticModelSharedData, InstancedStaticModelInitializationData, InstancedStaticModelInstanceData);

/*
*	Initializes this component.
*/
void InstancedStaticModelComponent::Initialize() NOEXCEPT
{

}

/*
*	Post-initializes this component.
*/
void InstancedStaticModelComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void InstancedStaticModelComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool InstancedStaticModelComponent::NeedsPreProcessing() const NOEXCEPT
{
	return true;
}

/*
*	Preprocessed initialization data an instance.
*/
void InstancedStaticModelComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Cache the initialization data.
	InstancedStaticModelInitializationData *const RESTRICT _initialization_data{ static_cast<InstancedStaticModelInitializationData *const RESTRICT>(initialization_data) };

	ASSERT(!_initialization_data->_WorldTransforms.Empty(), "Trying to add instanced static model entity without any transforms!");

	//Calculate the average cell for all transforms.
	Vector3<float32> average_cell{ 0.0f, 0.0f, 0.0f };

	for (const WorldTransform &world_transform : _initialization_data->_WorldTransforms)
	{
		for (uint8 i{ 0 }; i < 3; ++i)
		{
			average_cell[i] += static_cast<float32>(world_transform.GetCell()[i]);
		}
	}

	average_cell /= static_cast<float32>(_initialization_data->_WorldTransforms.Size());

	for (uint8 i{ 0 }; i < 3; ++i)
	{
		_initialization_data->_PreprocessedData._Cell[i] = CatalystBaseMath::Round<int32>(average_cell[i]);
	}

	//Create the transoformations buffer. Calculate the world space axis aligned bounding box in the process.
	DynamicArray<Matrix4x4> transformations;
	transformations.Reserve(_initialization_data->_WorldTransforms.Size());

	AxisAlignedBoundingBox3D axis_aligned_bounding_box;

	for (const WorldTransform &world_transform : _initialization_data->_WorldTransforms)
	{
		const Matrix4x4 transformation{ world_transform.ToRelativeMatrix4x4(_initialization_data->_PreprocessedData._Cell) };

		transformations.Emplace(transformation);

		AxisAlignedBoundingBox3D transformation_axis_aligned_bounding_box;
		RenderingUtilities::TransformAxisAlignedBoundingBox(_initialization_data->_ModelResource->_ModelSpaceAxisAlignedBoundingBox, transformation, &transformation_axis_aligned_bounding_box);

		axis_aligned_bounding_box.Expand(transformation_axis_aligned_bounding_box);
	}

	RenderingUtilities::CreateTransformationsBuffer(transformations, &_initialization_data->_PreprocessedData._TransformationsBuffer);

	//Set up the world space axis aligned bounding box.
	_initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox = WorldSpaceAxisAlignedBoundingBox3D
	(
		WorldPosition(_initialization_data->_PreprocessedData._Cell, axis_aligned_bounding_box._Minimum),
		WorldPosition(_initialization_data->_PreprocessedData._Cell, axis_aligned_bounding_box._Maximum)
	);

	//Set the number of transformations.
	_initialization_data->_PreprocessedData._NumberOfTransformations = static_cast<uint32>(transformations.Size());
}

/*
*	Creates an instance.
*/
void InstancedStaticModelComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	InstancedStaticModelInitializationData *const RESTRICT _initialization_data{ static_cast<InstancedStaticModelInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	InstancedStaticModelInstanceData &instance_data{ _InstanceData.Back() };

	instance_data._WorldSpaceAxisAlignedBoundingBox = _initialization_data->_PreprocessedData._WorldSpaceAxisAlignedBoundingBox;
	instance_data._ModelResource = _initialization_data->_ModelResource;
	instance_data._MaterialResources = _initialization_data->_MaterialResources;
	instance_data._Cell = _initialization_data->_PreprocessedData._Cell;
	instance_data._TransformationsBuffer = _initialization_data->_PreprocessedData._TransformationsBuffer;
	instance_data._NumberOfTransformations = _initialization_data->_PreprocessedData._NumberOfTransformations;
	instance_data._ModelFlags = _initialization_data->_ModelFlags;
	instance_data._ModelFadeData = _initialization_data->_ModelFadeData;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void InstancedStaticModelComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{

}

/*
*	Destroys an instance.
*/
void InstancedStaticModelComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	InstancedStaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

	//Destroy the transformations buffer.
	RenderingSystem::Instance->DestroyBuffer(&instance_data._TransformationsBuffer);

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 InstancedStaticModelComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void InstancedStaticModelComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE_RENDER;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 128;
}

/*
*	Runs before the given update phase.
*/
void InstancedStaticModelComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void InstancedStaticModelComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("InstancedStaticModelComponent::Update");

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
				InstancedStaticModelInstanceData &instance_data{ _InstanceData[instance_index] };

				//Do culling.
				{
					//Set visibility to true.
					instance_data._Visibility = true;

					//Cache the camera relative axis aligned bounding box.
					const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ instance_data._WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

					//Do distance culling.
					if (instance_data._ModelFadeData.Valid())
					{
						const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
						const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

						instance_data._Visibility = distance_squared < (instance_data._ModelFadeData.Get()._EndFadeOutDistance * instance_data._ModelFadeData.Get()._EndFadeOutDistance);
					}

					//Do frustum culling.
					if (instance_data._Visibility)
					{
						instance_data._Visibility = Culling::IsWithinFrustum(camera_relative_axis_aligned_bounding_box, *frustum);
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
*	Runs after the given update phase.
*/
void InstancedStaticModelComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}