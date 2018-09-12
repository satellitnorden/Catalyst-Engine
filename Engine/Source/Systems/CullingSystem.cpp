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
*	Pre-updates the culling system synchronously.
*/
void CullingSystem::PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute the relevant tasks.
	TaskSystem::Instance->ExecuteTask(&_CullingTasks[INDEX(CullingTask::UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes)]);

	//Wait for the relevant tasks to finish.
	_CullingTasks[INDEX(CullingTask::UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes)].WaitFor();
}

/*
*	Initializes the culling tasks.
*/
void CullingSystem::InitializeCullingTasks() NOEXCEPT
{
	//Initialize the update dynamic physical world space axis-aligned bounding boxes task.
	_CullingTasks[INDEX(CullingTask::UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes)]._Function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes();
	};
	_CullingTasks[INDEX(CullingTask::UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes)]._Arguments = nullptr;
}

/*
*	Update the world space axis-aligned bounding boxes of dynamic physical entities.
*/
void CullingSystem::UpdateDynamicPhysicalWorldSpaceAxisAlignedBoundingBoxes() const NOEXCEPT
{
	//Iterate over all dynamic physical components and update theior world space axis-aligned bounding boxes.
	const uint64 numberOfDynamicPhysicalComponents{ ComponentManager::GetNumberOfDynamicPhysicalComponents() };
	FrustumCullingComponent *RESTRICT cullingComponent{ ComponentManager::GetDynamicPhysicalFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetDynamicPhysicalTransformComponents() };

	for (uint64 i = 0; i < numberOfDynamicPhysicalComponents; ++i, ++cullingComponent, ++transformComponent)
	{
		cullingComponent->_WorldSpaceAxisAlignedBoundingBox._Minimum = cullingComponent->_ModelSpaceAxisAlignedBoundingBox._Minimum + transformComponent->_Position;
		cullingComponent->_WorldSpaceAxisAlignedBoundingBox._Maximum = cullingComponent->_ModelSpaceAxisAlignedBoundingBox._Maximum + transformComponent->_Position;
	}
}