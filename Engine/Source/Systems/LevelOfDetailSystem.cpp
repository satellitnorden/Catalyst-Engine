//Header file.
#include <Systems/LevelOfDetailSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/GrassCore.h>
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
	//Initialize the grass level of detail task.
	_GrassLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailGrass();
	};
	_GrassLevelOfDetailTask._Arguments = nullptr;
	_GrassLevelOfDetailTask._ExecutableOnSameThread = false;

	//Initialize the static models level of detail task.
	_StaticModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailStaticModels();
	};
	_StaticModelsLevelOfDetailTask._Arguments = nullptr;
	_StaticModelsLevelOfDetailTask._ExecutableOnSameThread = false;

	//Initialize the dynamic models level of detail task.
	_DynamicModelsLevelOfDetailTask._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailDynamicModels();
	};
	_DynamicModelsLevelOfDetailTask._Arguments = nullptr;
	_DynamicModelsLevelOfDetailTask._ExecutableOnSameThread = false;
}

/*
*	Updates the level of detail system during the render update phase.
*/
void LevelOfDetailSystem::RenderUpdate() NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_GrassLevelOfDetailTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_StaticModelsLevelOfDetailTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_DynamicModelsLevelOfDetailTask);
}

/*
*	Calculates level of detail for grass.
*/
void LevelOfDetailSystem::LevelOfDetailGrass() const NOEXCEPT
{
	PROFILING_SCOPE(LevelOfDetailSystem::LevelOfDetailGrass);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };

	//Iterate over all patches and determine their level of detail.
	const uint64 number_of_components{ ComponentManager::GetNumberOfGrassComponents() };
	GrassComponent *RESTRICT component{ ComponentManager::GetGrassGrassComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		//Cache the camera relative axis aligned bounding box.
		const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

		//Calculate the distance.
		const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
		const float32 distance{ Vector3<float32>::Length(camera_local_position - closest_position) };

		//Calculate the level of detail.
		component->_LevelOfDetail = CatalystBaseMath::Minimum<uint8>(static_cast<uint8>(distance / GrassConstants::LEVEL_OF_DETAIL_DISTANCE_FACTOR), GrassConstants::HIGHEST_LEVEL_OF_DETAIL);
	}
}

/*
*	Calculates level of detail for static models.
*/
void LevelOfDetailSystem::LevelOfDetailStaticModels() const NOEXCEPT
{
	PROFILING_SCOPE(LevelOfDetailSystem::LevelOfDetailStaticModels);

	//Cache the camera world transform.
	const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };

	//Cache the camera world to clip matrix.
	const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix() };

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
	PROFILING_SCOPE(LevelOfDetailSystem::LevelOfDetailDynamicModels);

	//Cache the camera world transform.
	const WorldTransform &camera_world_transform{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform() };

	//Cache the camera world to clip matrix.
	const Matrix4x4 *const RESTRICT camera_world_to_clip_matrix{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetViewMatrix() };

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