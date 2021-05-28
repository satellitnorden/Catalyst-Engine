//Header file.
#include <Systems/CullingSystem.h>

//Core.
#include <Core/General/Perceiver.h>

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
void CullingSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
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
	const Vector3<int32> perceiver_cell{ Perceiver::Instance->GetWorldTransform().GetCell() };
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfDynamicModelComponents() };
	DynamicModelComponent *RESTRICT component{ ComponentManager::GetDynamicModelDynamicModelComponents() };

	for (uint64 i = 0; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(perceiver_cell));
	}
}

/*
*	Culls static models.
*/
void CullingSystem::CullStaticModels() const NOEXCEPT
{
	//Cache data that will be used.
	const Vector3<int32> perceiver_cell{ Perceiver::Instance->GetWorldTransform().GetCell() };
	const float32 world_grid_size{ WorldSystem::Instance->GetWorldGridSize() };
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const uint64 number_of_components{ ComponentManager::GetNumberOfStaticModelComponents() };
	StaticModelComponent *RESTRICT component{ ComponentManager::GetStaticModelStaticModelComponents() };

	for (uint64 i{ 0 }; i < number_of_components; ++i, ++component)
	{
		component->_Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, component->_WorldSpaceAxisAlignedBoundingBox.GetRelativeAxisAlignedBoundingBox(perceiver_cell));
	}
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() const NOEXCEPT
{
	//Cache the frustum planes.
	const StaticArray<Vector4<float32>, 6> *const RESTRICT frustum_planes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all patches and determine their visibility.
	const DynamicArray<TerrainPatchInformation> *const RESTRICT patch_informations{ TerrainSystem::Instance->GetTerrainPatchInformations() };
	DynamicArray<TerrainPatchRenderInformation> *const RESTRICT patch_render_informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };
	const uint64 number_of_patch_informations{ patch_informations->Size() };

	for (uint64 i{ 0 }; i < number_of_patch_informations; ++i)
	{
		patch_render_informations->At(i)._Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, patch_informations->At(i)._AxisAlignedBoundingBox);
	}
}