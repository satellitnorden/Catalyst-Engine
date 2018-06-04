#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class LevelOfDetailSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(LevelOfDetailSystem);

	/*
	*	Default constructor.
	*/
	LevelOfDetailSystem() NOEXCEPT = default;

	/*
	*	Initializes the level of detail system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Post-initializes the level of detail system.
	*/
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Updates the level of detail system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Waits for the terrain level of detail to finish.
	*/
	void WaitForTerrainLevelOfDetail() const NOEXCEPT;

private:

	//Enumeration covering all level of detail tasks.
	enum class LevelOfDetailTask : uint8
	{
		Terrain,
		NumberOfLevelOfDetailTasks
	};

	//Enumeration covering all terrain buffers.
	enum class TerrainBuffer : uint8
	{
		TerrainBuffer_256x256,
		TerrainBuffer_128x128,
		TerrainBuffer_64x64,
		TerrainBuffer_32x32,
		TerrainBuffer_16x16,
		TerrainBuffer_8x8,
		NumberOfTerrainBuffers
	};

	//Container for all tasks.
	StaticArray<Task, INDEX(LevelOfDetailTask::NumberOfLevelOfDetailTasks)> tasks;

	//Container for all semaphores.
	StaticArray<Semaphore, INDEX(LevelOfDetailTask::NumberOfLevelOfDetailTasks)> semaphores;

	//Container for all terrain buffers.
	StaticArray<ConstantBufferHandle, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBuffers;

	//Container for all terrain buffer index offsets.
	StaticArray<uint64, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBufferIndexOffsets;

	//Container for all terrain buffer index counts.
	StaticArray<uint32, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBufferIndexCounts;

	/*
	*	Initializes the level of detail tasks.
	*/
	void InitializeLevelOfDetailTasks() NOEXCEPT;

	/*
	*	Initializes the terrain buffers.
	*/
	void InitializeTerrainBuffers() NOEXCEPT;

	/*
	*	Calculates the level of detail for the terrain.
	*/
	void CaltulateTerrainLevelOfDetail() NOEXCEPT;

};