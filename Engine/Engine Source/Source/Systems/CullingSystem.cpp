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

	//Initialize the terrain buffers.
	InitializeTerrainBuffers();
}

/*
*	Updates the culling system synchronously.
*/
void CullingSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//Fire off the culling tasks.
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::Terrain)]);
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::StaticPhysical)]);
	TaskSystem::Instance->ExecuteTask(tasks[INDEX(CullingTask::Vegetation)]);
}

/*
*	Initializes the culling tasks.
*/
void CullingSystem::InitializeCullingTasks() NOEXCEPT
{
	//Initialize the terrain culling task.
	tasks[INDEX(CullingTask::Terrain)].function = [](void *const RESTRICT)
	{
		CullingSystem::Instance->CullTerrain();
	};
	tasks[INDEX(CullingTask::Terrain)].arguments = nullptr;
	tasks[INDEX(CullingTask::Terrain)].semaphore = &semaphores[INDEX(CullingTask::Terrain)];

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
*	Initializes the terrain buffers.
*/
void CullingSystem::InitializeTerrainBuffers() NOEXCEPT
{
	constexpr StaticArray<uint32, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> resolutions
	{
		static_cast<uint32>(256),
		static_cast<uint32>(128),
		static_cast<uint32>(64),
		static_cast<uint32>(32),
		static_cast<uint32>(16),
		static_cast<uint32>(8),
		static_cast<uint32>(4),
		static_cast<uint32>(2)
	};

	for (uint8 i = 0; i < INDEX(TerrainBuffer::NumberOfTerrainBuffers); ++i)
	{
		DynamicArray<float> vertices;
		DynamicArray<uint32> indices;

		RenderingUtilities::GenerateTerrainPlane(resolutions[i], vertices, indices);

		const void *RESTRICT data[]{ vertices.Data(), indices.Data() };
		const uint64 dataSizes[]{ sizeof(float) * vertices.Size(), sizeof(uint32) * indices.Size() };

		terrainBuffers[i] = RenderingSystem::Instance->CreateConstantBuffer(data, dataSizes, 2);
		terrainBufferIndexOffsets[i] = sizeof(float) * vertices.Size();
		terrainBufferIndexCounts[i] = static_cast<uint32>(indices.Size());
	}
}

/*
*	Waits for the terrain culling to finish.
*/
void CullingSystem::WaitForTerrainCulling() const NOEXCEPT
{
	//Wait for the terrain culling to finish.
	semaphores[INDEX(CullingTask::Terrain)].WaitFor();
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
*	Culls terrain.
*/
void CullingSystem::CullTerrain() NOEXCEPT
{
	const uint64 numberOfTerrainComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	if (numberOfTerrainComponents == 0)
	{
		return;
	}

	const Vector3 cameraWorldPosition{ RenderingSystem::Instance->GetActiveCamera()->GetPosition() };
	const TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainComponents() };
	TerrainRenderComponent *RESTRICT renderComponent{ ComponentManager::GetTerrainRenderComponents() };

	for (uint64 i = 0; i < numberOfTerrainComponents; ++i, ++component, ++renderComponent)
	{
		const float distance{ Vector3::Length(cameraWorldPosition - component->terrainUniformData.terrainPosition) };

		if (distance < 10'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
		}

		else if (distance < 20'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
		}

		else if (distance < 30'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
		}

		else if (distance < 40'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
		}

		else if (distance < 50'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
		}

		else if (distance < 60'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
		}

		else if (distance < 70'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_4x4)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_4x4)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_4x4)];
		}

		else if (distance < 80'000.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_2x2)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_2x2)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_2x2)];
		}
	}
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