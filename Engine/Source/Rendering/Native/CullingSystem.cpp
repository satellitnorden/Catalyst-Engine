//Header file.
#include <Rendering/Native/CullingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/Culling.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

/*
*	Initializes the culling system.
*/
void CullingSystem::Initialize() NOEXCEPT
{
	//Initialize the dynamic models culling task.
	_DynamicModelsCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullDynamicModels();
	};
	_DynamicModelsCullingTask._Arguments = this;
	_DynamicModelsCullingTask._ExecutableOnSameThread = false;

	//Initialize the grass culling task.
	_GrassCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullGrass();
	};
	_GrassCullingTask._Arguments = this;
	_GrassCullingTask._ExecutableOnSameThread = false;

	//Initialize the instanced impostors culling task.
	_InstancedImpostorsCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullInstancedImpostors();
	};
	_InstancedImpostorsCullingTask._Arguments = this;
	_InstancedImpostorsCullingTask._ExecutableOnSameThread = false;

	//Initialize the instanced static models culling task.
	_InstancedStaticModelsCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullInstancedStaticModels();
	};
	_InstancedStaticModelsCullingTask._Arguments = this;
	_InstancedStaticModelsCullingTask._ExecutableOnSameThread = false;

	//Initialize the static models culling task.
	_StaticModelsCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullStaticModels();
	};
	_StaticModelsCullingTask._Arguments = this;
	_StaticModelsCullingTask._ExecutableOnSameThread = false;

	//Initialize the terrain culling task.
	_TerrainCullingTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<CullingSystem *const RESTRICT>(arguments)->CullTerrain();
	};
	_TerrainCullingTask._Arguments = this;
	_TerrainCullingTask._ExecutableOnSameThread = false;
}

/*
*	Updates the culling system during the pre render update phase.
*/
void CullingSystem::PreRenderUpdate() NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_DynamicModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_GrassCullingTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_InstancedImpostorsCullingTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_InstancedStaticModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_StaticModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_TerrainCullingTask);
}

/*
*	Culls dynamic models.
*/
void CullingSystem::CullDynamicModels() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullDynamicModels);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		//Reset the visibility flags.
		component->_VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

		//Do camera culling.
		{
			//Do frustum culling.
			if (!Culling::IsWithinFrustum(component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}

		//Do shadow map culling.
		for (uint32 shadow_map_data_index{ 0 }; shadow_map_data_index < RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData(); ++shadow_map_data_index)
		{
			const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(shadow_map_data_index) };

			//Do frustum culling.
			if (!Culling::IsWithinFrustum(component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), shadow_map_data._Frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::SHADOW_MAP_START << shadow_map_data_index);
			}
		}
	}
}

/*
*	Culls grass.
*/
void CullingSystem::CullGrass() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullGrass);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfGrassComponents() };
	GrassComponent *RESTRICT component{ ComponentManager::GetGrassGrassComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		//Reset the visibility flags.
		component->_VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

		//Cache the camera relative axis aligned bounding box.
		const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

		//Do distance culling.
		{
			const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
			const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

			if (distance_squared >= (component->_FadeOutDistance * component->_FadeOutDistance))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}

		//Do frustum culling.
		if (TEST_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA))
		{
			if (!Culling::IsWithinFrustum(camera_relative_axis_aligned_bounding_box, *frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}
	}
}

/*
*	Culls instanced impostors.
*/
void CullingSystem::CullInstancedImpostors() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullInstancedImpostors);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedImpostorComponents() };
	InstancedImpostorComponent *RESTRICT component{ ComponentManager::GetInstancedImpostorInstancedImpostorComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		//Reset the visibility flags.
		component->_VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

		//Cache the camera relative axis aligned bounding box.
		const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };

		//Do distance culling.
		{
			const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
			const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

			if (distance_squared >= (component->_EndFadeOutDistance * component->_EndFadeOutDistance))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}

		//Do frustum culling.
		if (TEST_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA))
		{
			if (!Culling::IsWithinFrustum(camera_relative_axis_aligned_bounding_box, *frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}
	}
}

/*
*	Culls instanced static models.
*/
void CullingSystem::CullInstancedStaticModels() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullInstancedStaticModels);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Vector3<float32> camera_local_position{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetLocalPosition() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedStaticModelComponents() };
	InstancedStaticModelComponent *RESTRICT component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		//Set visibility to true.
		component->_Visibility = true;

		//Cache the camera relative axis aligned bounding box.
		const AxisAlignedBoundingBox3D camera_relative_axis_aligned_bounding_box{ component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell) };
		
		//Do distance culling.
		if (component->_ModelFadeData.Valid())
		{
			const Vector3<float32> closest_position{ AxisAlignedBoundingBox3D::GetClosestPointInside(camera_relative_axis_aligned_bounding_box, camera_local_position) };
			const float32 distance_squared{ Vector3<float32>::LengthSquared(camera_local_position - closest_position) };

			component->_Visibility = distance_squared < (component->_ModelFadeData.Get()._EndFadeOutDistance * component->_ModelFadeData.Get()._EndFadeOutDistance);
		}

		//Do frustum culling.
		if (component->_Visibility)
		{
			component->_Visibility = Culling::IsWithinFrustum(camera_relative_axis_aligned_bounding_box, *frustum);
		}
	}
}

/*
*	Culls static models.
*/
void CullingSystem::CullStaticModels() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullStaticModels);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		//Reset the visibility flags.
		component->_VisibilityFlags = static_cast<VisibilityFlags>(UINT8_MAXIMUM);

		//Do camera culling.
		{
			//Do frustum culling.
			if (!Culling::IsWithinFrustum(component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::CAMERA);
			}
		}
		
		//Do shadow map culling.
		for (uint32 shadow_map_data_index{ 0 }; shadow_map_data_index < RenderingSystem::Instance->GetShadowsSystem()->GetNumberOfShadowMapData(); ++shadow_map_data_index)
		{
			const ShadowsSystem::ShadowMapData &shadow_map_data{ RenderingSystem::Instance->GetShadowsSystem()->GetShadowMapData(shadow_map_data_index) };
		
			//Do frustum culling.
			if (!Culling::IsWithinFrustum(component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), shadow_map_data._Frustum))
			{
				CLEAR_BIT(component->_VisibilityFlags, VisibilityFlags::SHADOW_MAP_START << shadow_map_data_index);
			}
		}
	}
}

/*
*	Culls a single terrain quad tree node.
*/
void CullTerrainQuadTreeNode
(
	const Vector3<float32> &world_grid_delta,
	const Frustum *const RESTRICT frustum,
	TerrainQuadTreeNode *const RESTRICT node
) NOEXCEPT
{
	//If this node is subdivided, cull each child node.
	if (node->IsSubdivided())
	{
		for (TerrainQuadTreeNode &child_node : node->_ChildNodes)
		{
			CullTerrainQuadTreeNode(world_grid_delta, frustum, &child_node);
		}
	}

	else
	{
		//Transform the axis aligned bounding box into the camera's cell.
		AxisAlignedBoundingBox3D transformed_axis_aligned_bounding_box
		{
			node->_AxisAlignedBoundingBox._Minimum + world_grid_delta,
			node->_AxisAlignedBoundingBox._Maximum + world_grid_delta
		};

		//Cull!
		node->_Visible = Culling::IsWithinFrustum(transformed_axis_aligned_bounding_box, *frustum);
	}
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() const NOEXCEPT
{
	PROFILING_SCOPE(CullingSystem::CullTerrain);

	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const Frustum *const RESTRICT frustum{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustum() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
	TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = Culling::IsWithinFrustum(component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell), *frustum);
	
		//If this terrain is visible, walk through the quad tree and determine the visibility of each node.
		if (component->_Visibility)
		{
			//Calculate the cell delta.
			const Vector3<int32> delta{ component->_WorldPosition.GetCell() - WorldSystem::Instance->GetCurrentWorldGridCell()};
			Vector3<float32> world_grid_delta;

			for (uint8 i{ 0 }; i < 3; ++i)
			{
				world_grid_delta[i] = static_cast<float32>(delta[i]) * WorldSystem::Instance->GetWorldGridSize();
			}

			//Start at the root node.
			CullTerrainQuadTreeNode(world_grid_delta, frustum, &component->_QuadTree._RootNode);
		}
	}
}