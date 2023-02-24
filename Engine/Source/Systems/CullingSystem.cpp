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
*	Culls terrain.
*/
void CullingSystem::CullTerrain() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> camera_cell{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetWorldTransform().GetCell() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ RenderingSystem::Instance->GetCameraSystem()->GetCurrentCamera()->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfTerrainComponents() };
	TerrainGeneralComponent *RESTRICT general_component{ ComponentManager::GetTerrainTerrainGeneralComponents() };
	TerrainRenderComponent *RESTRICT render_component{ ComponentManager::GetTerrainTerrainRenderComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++general_component, ++render_component)
	{
		render_component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, general_component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(camera_cell));
	}
}