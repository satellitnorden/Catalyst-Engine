#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Rendering.
#include <Rendering/Engine Layer/RenderingCore.h>

class CullingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CullingSystem);

	/*
	*	Initializes the culling system.
	*/
	void InitializeSystem() NOEXCEPT;

	/*
	*	Updates the culling system synchronously.
	*/
	void UpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Waits for the terrain culling to finish.
	*/
	void WaitForTerrainCulling() const NOEXCEPT;

	/*
	*	Waits for the static physical culling to finish.
	*/
	void WaitForStaticPhysicalCulling() const NOEXCEPT;

	/*
	*	Waits for the vegetation culling to finish.
	*/
	void WaitForVegetationCulling() const NOEXCEPT;

private:

	//Enumeration covering all culling tasks.
	enum class CullingTask : uint8
	{
		Terrain,
		StaticPhysical,
		Vegetation,
		NumberOfCullingTasks
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
		TerrainBuffer_4x4,
		TerrainBuffer_2x2,
		NumberOfTerrainBuffers
	};

	//Container for all tasks.
	StaticArray<Task, INDEX(CullingTask::NumberOfCullingTasks)> tasks;

	//Container for all semaphores.
	StaticArray<Semaphore, INDEX(CullingTask::NumberOfCullingTasks)> semaphores;

	//Container for all terrain buffers.
	StaticArray<ConstantBufferHandle, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBuffers;

	//Container for all terrain buffer index offsets.
	StaticArray<uint64, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBufferIndexOffsets;

	//Container for all terrain buffer index counts.
	StaticArray<uint32, INDEX(TerrainBuffer::NumberOfTerrainBuffers)> terrainBufferIndexCounts;

	/*
	*	Initializes the culling tasks.
	*/
	void InitializeCullingTasks() NOEXCEPT;

	/*
	*	Initializes the terrain buffers.
	*/
	void InitializeTerrainBuffers() NOEXCEPT;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() NOEXCEPT;

	/*
	*	Culls static physical.
	*/
	void CullStaticPhysical() NOEXCEPT;

	/*
	*	Culls vegetation.
	*/
	void CullVegetation() NOEXCEPT;

};