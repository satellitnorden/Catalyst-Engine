//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>

//Rendering.
#include <Rendering/Engine/RenderingUtilities.h>

//Systems.
#include <Systems/RenderingSystem.h>
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
	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	_CullingTasks[UNDERLYING(CullingTask::Terrain)]._Arguments = nullptr;

	_CullingTasks[UNDERLYING(CullingTask::Vegetation)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullVegetation();
	};
	_CullingTasks[UNDERLYING(CullingTask::Vegetation)]._Arguments = nullptr;
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
*	Culls terrain.
*/
void CullingSystem::CullTerrain() NOEXCEPT
{
	//Get the current view matrix.
	const Matrix4 *const RESTRICT viewMatrix{ RenderingSystem::Instance->GetViewMatrix() };

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

			//Test this patch's axis-aligned bounding box against the current view matrix.
			patchRenderInformation._Draw = RenderingUtilities::IsInViewFrustum(*viewMatrix, patchInformation._AxisAlignedBoundingBox);
		}
	}

	{
		//Iterate over all low detail terrain patches and cull them.
		StaticArray<TerrainPatchInformation, 32> *const RESTRICT patchInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchInformations() };
		StaticArray<TerrainPatchRenderInformation, 32> *const RESTRICT patchRenderInformations{ TerrainSystem::Instance->GetLowDetailTerrainPatchRenderInformations() };

		for (uint64 i = 0, size = patchInformations->Size(); i < size; ++i)
		{
			TerrainPatchInformation &patchInformation{ (*patchInformations)[i] };
			TerrainPatchRenderInformation &patchRenderInformation{ (*patchRenderInformations)[i] };

			//If this patch is invalid, no need to check it.
			if (!(*patchInformations)[i]._Valid)
			{
				continue;
			}

			//Test this patch's axis-aligned bounding box against the current view matrix.
			patchRenderInformation._Draw = RenderingUtilities::IsInViewFrustum(*viewMatrix, patchInformation._AxisAlignedBoundingBox);
		}
	}
}

/*
*	Culls vegetation.
*/
void CullingSystem::CullVegetation() NOEXCEPT
{
	//Get the camera position.
	const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
	const Vector3 cameraPosition{ camera->GetPosition() };

	//Iterate over all grass vegetation type informations, and cull the grid points that is too far away from the camera.
	for (GrassVegetationTypeInformation &information : *VegetationSystem::Instance->GetGrassVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Get this patch's world position.
			const Vector3 patchPosition{ GridPoint2::GridPointToWorldPosition(information._PatchInformations[i]._GridPoint, information._Properties._CutoffDistance * 2.0f) };

			//Perform the distance test.
			const bool distanceTest{	CatalystBaseMath::Absolute(cameraPosition._X - patchPosition._X) < information._Properties._CutoffDistance * 2.0f
										&& CatalystBaseMath::Absolute(cameraPosition._Z - patchPosition._Z) < information._Properties._CutoffDistance * 2.0f };

			//If the distance test succeeded, this patch should be drawn.
			if (distanceTest)
			{
				information._PatchRenderInformations[i]._Draw = true;
			}

			else
			{
				information._PatchRenderInformations[i]._Draw = false;
			}
		}
	}
}