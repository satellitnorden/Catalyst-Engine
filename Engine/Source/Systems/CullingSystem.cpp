//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize all culling tasks.
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
*	Culls vegetation.
*/
void CullingSystem::CullVegetation() NOEXCEPT
{
	//Get the camera position.
	const CameraEntity *const RESTRICT camera{ RenderingSystem::Instance->GetActiveCamera() };
	const Vector3 cameraPosition{ camera->GetPosition() };

	//Iterate over alll vegetion type informations, and cull the grid points that is too far away from the camera.
	for (VegetationTypeInformation &information : *VegetationSystem::Instance->GetVegetationTypeInformations())
	{
		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			//If this patch isn't even valid, don't test it.
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			//Get this patch's world position.
			const Vector3 patchPosition{ GridPoint::GridPointToWorldPosition(information._PatchInformations[i]._GridPoint, information._Properties._CutoffDistance * 2.0f) };

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