//Header file.
#include <Systems/LevelOfDetailSystem.h>

//Rendering.
#include <Rendering/Engine/Viewer.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>
#include <Systems/VegetationSystem.h>

//Singleton definition.
DEFINE_SINGLETON(LevelOfDetailSystem);

/*
*	Initializes the level of detail system.
*/
void LevelOfDetailSystem::InitializeSystem() NOEXCEPT
{
	//Initialize all level of detail tasks.
	_LevelOfDetailTasks[UNDERLYING(LevelOfDetailTask::GrassVegetation)]._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailGrassVegetation();
	};
	_LevelOfDetailTasks[UNDERLYING(LevelOfDetailTask::GrassVegetation)]._Arguments = nullptr;

	_LevelOfDetailTasks[UNDERLYING(LevelOfDetailTask::SolidVegetation)]._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->LevelOfDetailSolidVegetation();
	};
	_LevelOfDetailTasks[UNDERLYING(LevelOfDetailTask::SolidVegetation)]._Arguments = nullptr;
}

/*
*	Updates the level of detail sustem during the culling update phase.
*/
void LevelOfDetailSystem::CullingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Fire off all level of detail tasks.
	for (Task &task : _LevelOfDetailTasks)
	{
		TaskSystem::Instance->ExecuteTask(&task);
	}
}

/*
*	Calculates level of detail for grass vegetation.
*/
void LevelOfDetailSystem::LevelOfDetailGrassVegetation() NOEXCEPT
{
	//Iterate over all grass vegetation type informations and determine their level of detail.
	DynamicArray<GrassVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetGrassVegetationTypeInformations() };

	for (GrassVegetationTypeInformation &information : *informations)
	{
		//Cache the grid size.
		const float gridSize{ information._Properties._CutoffDistance * VegetationConstants::GRASS_VEGETATION_GRID_SIZE };

		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			information._PatchRenderInformations[i]._LevelOfDetail = VegetationLevelOfDetail::High;
		}
	}
}

/*
*	Calculates level of detail for solid vegetation.
*/
void LevelOfDetailSystem::LevelOfDetailSolidVegetation() NOEXCEPT
{
	//Iterate over all solid vegetation type informations and determine their level of detail.
	DynamicArray<SolidVegetationTypeInformation> *const RESTRICT informations{ VegetationSystem::Instance->GetSolidVegetationTypeInformations() };

	for (SolidVegetationTypeInformation &information : *informations)
	{
		//Cache the grid size.
		const float gridSize{ information._Properties._CutoffDistance * VegetationConstants::SOLID_VEGETATION_GRID_SIZE };

		for (uint64 i = 0, size = information._PatchInformations.Size(); i < size; ++i)
		{
			if (!information._PatchInformations[i]._Valid)
			{
				continue;
			}

			const Vector3<float> gridMiddlePoint{ GridPoint2::GridPointToWorldPosition(information._PatchInformations[i]._GridPoint, gridSize) };
			const float distanceToGridPoint{ Vector3<float>::LengthSquaredXZ(Viewer::Instance->GetPosition() - gridMiddlePoint) };

			if (distanceToGridPoint < information._Properties._MediumDetailDistance * information._Properties._MediumDetailDistance)
			{
				information._PatchRenderInformations[i]._LevelOfDetail = VegetationLevelOfDetail::High;
			}

			else if (distanceToGridPoint < information._Properties._LowDetailDistance * information._Properties._LowDetailDistance)
			{
				information._PatchRenderInformations[i]._LevelOfDetail = VegetationLevelOfDetail::Medium;
			}

			else
			{
				information._PatchRenderInformations[i]._LevelOfDetail = VegetationLevelOfDetail::Low;
			}
		}
	}
}