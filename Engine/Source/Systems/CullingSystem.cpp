//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/WorldSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::Initialize() NOEXCEPT
{
	//Initialize the dynamic models culling task.
	_DynamicModelsCullingTask._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullDynamicModels();
	};
	_DynamicModelsCullingTask._Arguments = nullptr;
	_DynamicModelsCullingTask._ExecutableOnSameThread = true;

	//Initialize the instanced static models culling task.
	_InstancedStaticModelsCullingTask._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullInstancedStaticModels();
	};
	_InstancedStaticModelsCullingTask._Arguments = nullptr;
	_InstancedStaticModelsCullingTask._ExecutableOnSameThread = true;

	//Initialize the static models culling task.
	_StaticModelsCullingTask._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullStaticModels();
	};
	_StaticModelsCullingTask._Arguments = nullptr;
	_StaticModelsCullingTask._ExecutableOnSameThread = true;

	//Initialize the terrain culling task.
	_TerrainCullingTask._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	_TerrainCullingTask._Arguments = nullptr;
	_TerrainCullingTask._ExecutableOnSameThread = true;
}

/*
*	Updates the culling system during the render update phase.
*/
void CullingSystem::RenderUpdate() NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(&_DynamicModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(&_InstancedStaticModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(&_StaticModelsCullingTask);
	TaskSystem::Instance->ExecuteTask(&_TerrainCullingTask);
}

/*
*	Culls dynamic models.
*/
void CullingSystem::CullDynamicModels() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell));
	}
}

/*
*	Culls instanced static models.
*/
void CullingSystem::CullInstancedStaticModels() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfInstancedStaticModelComponents() };
	InstancedStaticModelComponent *RESTRICT component{ ComponentManager::GetInstancedStaticModelInstancedStaticModelComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell));
	}
}

/*
*	Culls static models.
*/
void CullingSystem::CullStaticModels() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell));
	}
}

/*
*	Culls a single terrain quad tree node.
*/
void CullTerrainQuadTreeNode
(
	const Vector3<float32> &world_grid_delta,
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes,
	TerrainQuadTreeNode *const RESTRICT node
) NOEXCEPT
{
	//If this node is subdivided, cull each child node.
	if (node->IsSubdivided())
	{
		for (TerrainQuadTreeNode &child_node : node->_ChildNodes)
		{
			CullTerrainQuadTreeNode(world_grid_delta, frustum_planes, &child_node);
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
		node->_Visible = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, transformed_axis_aligned_bounding_box);
	}
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
	TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell));
	
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
			CullTerrainQuadTreeNode(world_grid_delta, frustum_planes, &component->_QuadTree._RootNode);
		}
	}
}