//Header file.
#include <Systems/LevelOfDetailSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelOfDetailSystem);

/*
*	Initializes the level of detail system.
*/
void LevelOfDetailSystem::Initialize() NOEXCEPT
{
	//Initialize the static models level of detail task.
	_StaticModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailStaticModels();
	};
	_StaticModelsLevelOfDetailTask._Arguments = nullptr;
	_StaticModelsLevelOfDetailTask._ExecutableOnSameThread = true;

	//Initialize the dynamic models level of detail task.
	_DynamicModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailDynamicModels();
	};
	_DynamicModelsLevelOfDetailTask._Arguments = nullptr;
	_DynamicModelsLevelOfDetailTask._ExecutableOnSameThread = true;
}

/*
*	Updates the level of detail system during the render update phase.
*/
void LevelOfDetailSystem::RenderUpdate() NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(&_StaticModelsLevelOfDetailTask);
	TaskSystem::Instance->ExecuteTask(&_DynamicModelsLevelOfDetailTask);
}

/*
*	Calculates level of detail for static models.
*/
void LevelOfDetailSystem::LevelOfDetailStaticModels() const NOEXCEPT
{
	//Cache the camera world transform.
	const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCurrentCamera()->GetWorldTransform() };

	//Cache the camera world to clip matrix.
	const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCurrentCamera()->GetViewMatrix() };

	//Iterate over all static model components and calculate their level of detail.
	const uint64 number_of_static_model_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	for (uint64 component_index{ 0 }; component_index < number_of_static_model_components; ++component_index, ++component)
	{
		for (uint64 mesh_index{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
		{
			//If the mesh used only has one level of detail, skip it.
			if (component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() == 1)
			{
				component->_LevelOfDetailIndices[mesh_index] = 0;

				continue;
			}

			//Retrieve the relative axis aligned bounding box.
			const AxisAlignedBoundingBox3D relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_world_transform.GetCell()) };

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
			float32 screen_coverage{ CatalystBaseMath::Minimum<float32>((maximum_screen_coordinate._X - minimum_screen_coordinate._X) * (maximum_screen_coordinate._Y - minimum_screen_coordinate._Y), 1.0f) };

			//Bias the screen coverage.
			screen_coverage = CatalystBaseMath::Bias(screen_coverage, _ScreenCoverageBias);

			//Calculate the level of detail index.
			component->_LevelOfDetailIndices[mesh_index] = static_cast<uint32>((1.0f - screen_coverage) * static_cast<float32>(component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() - 1));
		}
	}
}

/*
*	Calculates level of detail for dynamice models.
*/
void LevelOfDetailSystem::LevelOfDetailDynamicModels() const NOEXCEPT
{
	//Cache the camera world transform.
	const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCurrentCamera()->GetWorldTransform() };

	//Cache the camera world to clip matrix.
	const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCurrentCamera()->GetViewMatrix() };

	//Iterate over all dynamic model components and calculate their level of detail.
	const uint64 number_of_dynamic_model_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	for (uint64 component_index{ 0 }; component_index < number_of_dynamic_model_components; ++component_index, ++component)
	{
		for (uint64 mesh_index{ 0 }, size{ component->_ModelResource->_Meshes.Size() }; mesh_index < size; ++mesh_index)
		{
			//If the mesh used only has one level of detail, skip it.
			if (component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() == 1)
			{
				component->_LevelOfDetailIndices[mesh_index] = 0;

				continue;
			}

			//Retrieve the relative axis aligned bounding box.
			const AxisAlignedBoundingBox3D relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_world_transform.GetCell()) };

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
			float32 screen_coverage{ CatalystBaseMath::Minimum<float32>((maximum_screen_coordinate._X - minimum_screen_coordinate._X) * (maximum_screen_coordinate._Y - minimum_screen_coordinate._Y), 1.0f) };

			//Bias the screen coverage.
			screen_coverage = CatalystBaseMath::Bias(screen_coverage, _ScreenCoverageBias);

			//Calculate the level of detail index.
			component->_LevelOfDetailIndices[mesh_index] = static_cast<uint32>((1.0f - screen_coverage) * static_cast<float32>(component->_ModelResource->_Meshes[mesh_index]._MeshLevelOfDetails.Size() - 1));
		}
	}
}