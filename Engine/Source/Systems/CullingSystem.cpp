//Header file.
#include <Systems/CullingSystem.h>

//Core.
#include <Core/General/Perceiver.h>

//Rendering.
#include <Rendering/Native/RenderingUtilities.h>

//Systems.
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::Initialize() NOEXCEPT
{
	//Initialize the terrain culling task.
	_TerrainCullingTask._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	_TerrainCullingTask._Arguments = nullptr;
}

/*
*	Updates the culling system during the render update phase.
*/
void CullingSystem::RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute all tasks.
	TaskSystem::Instance->ExecuteTask(&_TerrainCullingTask);
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() const NOEXCEPT
{
	//Cache the frustum planes.
	const StaticArray<Vector4<float>, 6> *const RESTRICT frustum_planes{ Perceiver::Instance->GetFrustumPlanes() };

	//Iterate over all patches and determine if they are in the view frustum.
	const DynamicArray<TerrainPatchInformation> *const RESTRICT patch_informations{ TerrainSystem::Instance->GetTerrainPatchInformations() };
	DynamicArray<TerrainPatchRenderInformation> *const RESTRICT patch_render_informations{ TerrainSystem::Instance->GetTerrainPatchRenderInformations() };
	const uint64 number_of_patch_informations{ patch_informations->Size() };

	for (uint64 i{ 0 }; i < number_of_patch_informations; ++i)
	{
		patch_render_informations->At(i)._Visibility = RenderingUtilities::IsWithinViewFrustum(*frustum_planes, patch_informations->At(i)._AxisAlignedBoundingBox);
	}
}