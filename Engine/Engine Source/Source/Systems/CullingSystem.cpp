//Header file.
#include <Systems/CullingSystem.h>

//Components.
#include <Components/ComponentManager.h>

//Entities.
#include <Entities/CameraEntity.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingUtilities.h>

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
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::StaticPhysical)]);
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::Vegetation)]);
}

/*
*	Initializes the culling tasks.
*/
void CullingSystem::InitializeCullingTasks() NOEXCEPT
{
	//Initialize the static physical culling task.
	tasks[INDEX(CullingTask::StaticPhysical)].function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullStaticPhysical();
	};
	tasks[INDEX(CullingTask::StaticPhysical)].arguments = nullptr;
	tasks[INDEX(CullingTask::StaticPhysical)].semaphore = &semaphores[INDEX(CullingTask::StaticPhysical)];

	//Initialize the vegetation culling task.
	tasks[INDEX(CullingTask::Vegetation)].function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullVegetation();
	};
	tasks[INDEX(CullingTask::Vegetation)].arguments = nullptr;
	tasks[INDEX(CullingTask::Vegetation)].semaphore = &semaphores[INDEX(CullingTask::Vegetation)];
}

/*
*	Waits for the static physical culling to finish.
*/
void CullingSystem::WaitForStaticPhysicalCulling() const NOEXCEPT
{
	//Wait for the static physical culling to finish.
	semaphores[INDEX(CullingTask::StaticPhysical)].WaitFor();
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
*	Culls static physical.
*/
void CullingSystem::CullStaticPhysical() NOEXCEPT
{
	//Get the view matrix.
	const Matrix4& viewMatrix{ *RenderingSystem::Instance->GetViewMatrix() };

	//Iterate over all static physical components to check if they are in the view frustum.
	const uint64 numberOfStaticPhysicalComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };
	FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents() };

	for (uint64 i = 0; i < numberOfStaticPhysicalComponents; ++i, ++frustumCullingComponent, ++transformComponent)
	{
		//Cache relevant data.
		const Vector3& position = transformComponent->position;
		const Vector3& scale = transformComponent->scale;
		const float biggestScale = CatalystMath::Maximum(scale.X, CatalystMath::Maximum(scale.Y, scale.Z));
		const float scaledExtent = frustumCullingComponent->axisAlignedBoundingBox.maximum.X * biggestScale;

		StaticArray<Vector4, 8> corners;

		corners[0] = Vector4(-scaledExtent, -scaledExtent, -scaledExtent, 1.0f);
		corners[1] = Vector4(-scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[2] = Vector4(scaledExtent, scaledExtent, -scaledExtent, 1.0f);
		corners[3] = Vector4(scaledExtent, -scaledExtent, -scaledExtent, 1.0f);

		corners[4] = Vector4(-scaledExtent, -scaledExtent, scaledExtent, 1.0f);
		corners[5] = Vector4(-scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[6] = Vector4(scaledExtent, scaledExtent, scaledExtent, 1.0f);
		corners[7] = Vector4(scaledExtent, -scaledExtent, scaledExtent, 1.0f);

		for (uint8 i = 0; i < 8; ++i)
		{
			corners[i] += Vector4(position.X, position.Y, position.Z, 0.0f);

			corners[i] = viewMatrix * corners[i];

			corners[i].X /= corners[i].W;
			corners[i].Y /= corners[i].W;
			corners[i].Z /= corners[i].W;
		}

		frustumCullingComponent->isInViewFrustum = RenderingUtilities::IsCubeWithinViewFrustum(corners);
	}
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