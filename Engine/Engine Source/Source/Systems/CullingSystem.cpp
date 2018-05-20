//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>

//Systems.
#include <Systems/RenderingSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(CullingSystem);

/*
*	Initializes the culling system.
*/
void CullingSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the culling tasks.
	InitializeCullingTasks();
}

/*
*	Updates the culling system synchronously.
*/
void CullingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Fire off the culling tasks.
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::Vegetation)]);
}

/*
*	Initializes the culling tasks.
*/
void CullingSystem::InitializeCullingTasks() NOEXCEPT
{
	//Initialize the vegetation culling task.
	tasks[INDEX(CullingTask::Vegetation)].function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullVegetation();
	};
	tasks[INDEX(CullingTask::Vegetation)].arguments = nullptr;
	tasks[INDEX(CullingTask::Vegetation)].semaphore = &semaphores[INDEX(CullingTask::Vegetation)];
}

/*
*	Waits for the vegetation culling to finish.
*/
void CullingSystem::WaitForVegetationCulling() const NOEXCEPT
{
	//Wait for the vegetation culling to finish.
	semaphores[INDEX(CullingTask::Vegetation)].WaitFor();
}

/*
*	Culls vegetation.
*/
void CullingSystem::CullVegetation() NOEXCEPT
{
	//Go through all vegetation components and update their culling
	const uint64 numberOfVegetationComponents{ ComponentManager::GetNumberOfVegetationComponents() };

	//If there are none, just return.
	if (numberOfVegetationComponents == 0)
	{
		return;
	}

	VegetationComponent *RESTRICT renderComponent{ ComponentManager::GetVegetationComponents() };
	const VegetationCullingComponent *RESTRICT cullingComponent{ ComponentManager::GetVegetationCullingComponents() };

	const Vector3 &cameraWorldPosition{ RenderingSystem::Instance->GetActiveCamera()->GetPosition() };

	for (uint64 i = 0; i < numberOfVegetationComponents; ++i, ++renderComponent, ++cullingComponent)
	{
		for (uint64 i = 0, size = renderComponent->shouldDrawGridCell.Size(); i < size; ++i)
		{
			renderComponent->shouldDrawGridCell[i] = CatalystMath::Absolute(cameraWorldPosition.X - cullingComponent->gridCellCenterLocations[i].X) <= cullingComponent->cutoffDistance &&
				CatalystMath::Absolute(cameraWorldPosition.Z - cullingComponent->gridCellCenterLocations[i].Y) <= cullingComponent->cutoffDistance;
		}
	}
}