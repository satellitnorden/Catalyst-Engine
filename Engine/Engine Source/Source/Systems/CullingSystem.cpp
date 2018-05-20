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
*	Culls static physical.
*/
void CullingSystem::CullStaticPhysical() NOEXCEPT
{
	/*
	//Calulate the view matrix.
	Vector3 cameraWorldPosition = activeCamera->GetPosition();
	Vector3 forwardVector = activeCamera->GetForwardVector();
	Vector3 upVector = activeCamera->GetUpVector();

	Matrix4 cameraMatrix = Matrix4::LookAt(cameraWorldPosition, cameraWorldPosition + forwardVector, upVector);
	Matrix4 viewMatrix{ projectionMatrix * cameraMatrix };

	//Iterate over all static physical entity components to check if they are in the view frustum.
	const uint64 numberOfPhysicalEntityComponents{ ComponentManager::GetNumberOfStaticPhysicalComponents() };
	FrustumCullingComponent *RESTRICT frustumCullingComponent{ ComponentManager::GetStaticPhysicalFrustumCullingComponents() };
	const TransformComponent *RESTRICT transformComponent{ ComponentManager::GetStaticPhysicalTransformComponents() };

	for (uint64 i = 0; i < numberOfPhysicalEntityComponents; ++i, ++frustumCullingComponent, ++transformComponent)
	{
		//Make a local copy of the static physical entity's position.
		const Vector3 position = transformComponent->position;
		const Vector3 scale = transformComponent->scale;
		const float biggestScale = CatalystMath::Maximum(scale.X, CatalystMath::Maximum(scale.Y, scale.Z));
		const float scaledExtent = frustumCullingComponent->axisAlignedBoundingBox.maximum.X * biggestScale;

		Vector4 corners[8];

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
	*/
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