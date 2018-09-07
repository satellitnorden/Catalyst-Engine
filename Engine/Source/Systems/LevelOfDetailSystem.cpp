//Header file.
#include <Systems/LevelOfDetailSystem.h>

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
DEFINE_SINGLETON(LevelOfDetailSystem);

/*
*	Initializes the level of detail system.
*/
void LevelOfDetailSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the level of detail tasks.
	InitializeLevelOfDetailTasks();
}

/*
*	Post-initializes the level of detail system.
*/
void LevelOfDetailSystem::PostInitializeSystem() NOEXCEPT
{
	//Initialize the terrain buffers.
	InitializeTerrainBuffers();
}

/*
*	Updates the level of detail system synchronously.
*/
void LevelOfDetailSystem::UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Fire off the level of detail tasks.
	TaskSystem::Instance->ExecuteTask(&_Tasks[INDEX(LevelOfDetailTask::Terrain)]);
}

/*
*	Waits for the terrain level of detail to finish.
*/
void LevelOfDetailSystem::WaitForTerrainLevelOfDetail() const NOEXCEPT
{
	//Wait for the terrain level of detail to finish.
	_Tasks[INDEX(LevelOfDetailTask::Terrain)].WaitFor();
}

/*
*	Initializes the level of detail tasks.
*/
void LevelOfDetailSystem::InitializeLevelOfDetailTasks() NOEXCEPT
{
	//Initialize the terrain level of detail task.
	_Tasks[INDEX(LevelOfDetailTask::Terrain)]._Function = [](void *const RESTRICT)
	{
		LevelOfDetailSystem::Instance->CaltulateTerrainLevelOfDetail();
	};
	_Tasks[INDEX(LevelOfDetailTask::Terrain)]._Arguments = nullptr;
}

/*
*	Initializes the terrain buffers.
*/
void LevelOfDetailSystem::InitializeTerrainBuffers() NOEXCEPT
{
	//Initialize the terrain buffers.
	constexpr StaticArray<uint32, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> resolutions
	{
		static_cast<uint32>(256),
		static_cast<uint32>(128),
		static_cast<uint32>(64),
		static_cast<uint32>(32),
		static_cast<uint32>(16),
		static_cast<uint32>(8)
	};

	for (uint8 i = 0; i < INDEX(TerrainBuffer::NumberOfTerrainBuffers); ++i)
	{
		DynamicArray<float> vertices;
		DynamicArray<uint32> indices;

		RenderingUtilities::GenerateTerrainPlane(resolutions[i], vertices, indices);

		const void *RESTRICT data[]{ vertices.Data(), indices.Data() };
		const uint64 dataSizes[]{ sizeof(float) * vertices.Size(), sizeof(uint32) * indices.Size() };

		_TerrainBuffers[i] = RenderingSystem::Instance->CreateConstantBuffer(data, dataSizes, 2);
		_TerrainBufferIndexOffsets[i] = sizeof(float) * vertices.Size();
		_TerrainBufferIndexCounts[i] = static_cast<uint32>(indices.Size());
	}
}

/*
*	Calculates the level of detail for the terrain.
*/
void LevelOfDetailSystem::CaltulateTerrainLevelOfDetail() NOEXCEPT
{
	//Iterate over all terrain components and calculate their level of detail.
	const uint64 numberOfTerrainComponents{ ComponentManager::GetNumberOfTerrainComponents() };

	//If there's none to calculate, just return.
	if (numberOfTerrainComponents == 0)
	{
		return;
	}

	//Cache relevant data.
	const Vector3 cameraWorldPosition{ RenderingSystem::Instance->GetActiveCamera()->GetPosition() };
	const TerrainComponent *RESTRICT component{ ComponentManager::GetTerrainTerrainComponents() };
	TerrainRenderComponent *RESTRICT renderComponent{ ComponentManager::GetTerrainTerrainRenderComponents() };

	for (uint64 i = 0; i < numberOfTerrainComponents; ++i, ++component, ++renderComponent)
	{
		/*
		const float distance{ Vector3::Length(cameraWorldPosition - component->terrainUniformData.terrainPosition) };

		if (distance < component->terrainUniformData.terrainSize)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_256x256)];
		}

		else if (distance < component->terrainUniformData.terrainSize * 2.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_128x128)];
		}

		else if (distance < component->terrainUniformData.terrainSize * 3.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_64x64)];
		}

		else if (distance < component->terrainUniformData.terrainSize * 4.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_32x32)];
		}

		else if (distance < component->terrainUniformData.terrainSize * 5.0f)
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
		}

		else
		{
			renderComponent->vertexAndIndexBuffer = terrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
			renderComponent->indexBufferOffset = terrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
			renderComponent->indexCount = terrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_8x8)];
		}
		*/

		renderComponent->_Buffer = _TerrainBuffers[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
		renderComponent->_IndexOffset = _TerrainBufferIndexOffsets[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
		renderComponent->_IndexCount = _TerrainBufferIndexCounts[INDEX(TerrainBuffer::TerrainBuffer_16x16)];
	}
}