//Header file.
#include <Rendering/Native/CullingSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>
#include <Components/Components/StaticModelComponent.h>

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
	TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &_TerrainCullingTask);
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