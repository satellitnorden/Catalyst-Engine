//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/LightingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/TerrainSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize all culling tasks.
	_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullGrassVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::SolidVegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullSolidVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::SolidVegetation)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Arguments = nullptr;
}

/*
*	Updates the culling sustem during the culling update phase.
*/
void CullingSystem::CullingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Fire off all culling tasks.
	for (Task &task : _CullingTasks)
	{
		TaskSystem::Instance->ExecuteTask(&task);
	}
}

/*
*	Culls grass vegetation.
*/
void CullingSystem::CullGrassVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4, 6> *const RESTRICT frustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the viewer.
	for (GrassVegetationTypeInformation &information : *VegetationSystem::Instance->GetGrassVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}
		}
	}
}

/*
*	Culls solid vegetation.
*/
void CullingSystem::CullSolidVegetation() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4, 6> *const RESTRICT directionalLightFrustumPlanes{ LightingSystem::Instance->GetDirectionalLight()->GetFrustumPlanes() };
	const StaticArray<Vector4, 6> *const RESTRICT viewerFrustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the viewer.
	for (SolidVegetationTypeInformation &information : *VegetationSystem::Instance->GetSolidVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*viewerFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::Viewer);
			}

			if (RenderingUtilities::IsWithinViewFrustum(*directionalLightFrustumPlanes, information._PatchInformations[i]._AxisAlignedBoundingBox))
			{
				SET_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
			}

			else
			{
				CLEAR_BIT(information._PatchRenderInformations[i]._Visibility, VisibilityFlag::DirectionalLight);
			}
		}
	}
}

/*
*	Culls terrain.
*/
void CullingSystem::CullTerrain() NOEXCEPT
{
	//Get the current frustum planes.
	const StaticArray<Vector4, 6> *const RESTRICT frustumPlanes{ Viewer::Instance->GetFrustumPlanes() };

	{
		//Iterate over all high detail terrain patches and cull them.
		StaticArray<TerrainPatchInformation, 9> *const RESTRICT patchInformations{ TerrainSystem::Instance->GetHighDetailTerrainPatchInformations() };
		StaticArray<TerrainPatchRenderInformation, 9> *const RESTRICT patchRenderInformations{ TerrainSystem::Instance->GetHighDetailTerrainPatchRenderInformations() };

		for (uint64 i = 0, size = patchInformations->Size(); i < size; ++i)
		{
			TerrainPatchInformation &patchInformation{ (*patchInformations)[i] };
			TerrainPatchRenderInformation &patchRenderInformation{ (*patchRenderInformations)[i] };

			//If this patch is invalid, no need to check it.
			if (!(*patchInformations)[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, patchInformation._AxisAlignedBoundingBox))
			{
				SET_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
			}
		}
	}

	{
		//Iterate over all low detail terrain patches and cull them.
		StaticArray<TerrainPatchInformation, 56> *const RESTRICT patchInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchInformations() };
		StaticArray<TerrainPatchRenderInformation, 56> *const RESTRICT patchRenderInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchRenderInformations() };

		for (uint64 i = 0, size = patchInformations->Size(); i < size; ++i)
		{
			TerrainPatchInformation &patchInformation{ (*patchInformations)[i] };
			TerrainPatchRenderInformation &patchRenderInformation{ (*patchRenderInformations)[i] };

			//If this patch is invalid, no need to check it.
			if (!(*patchInformations)[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current frustum planes.
			if (RenderingUtilities::IsWithinViewFrustum(*frustumPlanes, patchInformation._AxisAlignedBoundingBox))
			{
				SET_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
			}

			else
			{
				CLEAR_BIT(patchRenderInformation._Visibility, VisibilityFlag::Viewer);
			}
		}
	}
}