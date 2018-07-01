#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Clairvoyant.
#include <World/SuggestedWorldChunk.h>
#include <World/WorldChunk.h>
#include <World/WorldScanningData.h>

//Forward declarations.
class DirectionalLightEntity;
class Vector3;

class WorldArchitect final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldArchitect);
	
	//Defines the size of the world chunk grid.
	static constexpr uint8 WORLD_CHUNK_GRID_SIZE{ 15 };

	/*
	*	Default constructor.
	*/
	WorldArchitect() NOEXCEPT = default;

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the world architect.
	*/
	void Update(const float deltaTime) NOEXCEPT;

private:

	//Enumeration covering all world architects states.
	enum class WorldArchitectState : uint8
	{
		Idling,
		Scanning,
		Generating
	};

	//The task.
	Task task;

	//The semaphore.
	Semaphore semaphore;

	//The current world architect state.
	WorldArchitectState currentState{ WorldArchitectState::Idling };

	//The world scanning data.
	WorldScanningData scanningData;

	//The suggested world chunk.
	SuggestedWorldChunk suggestedWorldChunk;

	//The world chunks.
	StaticArray<WorldChunk, WORLD_CHUNK_GRID_SIZE * WORLD_CHUNK_GRID_SIZE> worldChunks;

	/*
	*	Initializes the task.
	*/
	void InitializeTask() NOEXCEPT;

	/*
	*	Updates the idling state.
	*/
	void UpdateIdling() NOEXCEPT;

	/*
	*	Updates the scanning state.
	*/
	void UpdateScanning() NOEXCEPT;

	/*
	*	Updates the generating state.
	*/
	void UpdateGenerating() NOEXCEPT;

	/*
	*	Scans the world, generating chunks to generate and chunks to destroy.
	*/
	void Scan() NOEXCEPT;

	/*
	*	Generates a new chunk.
	*/
	void Generate() NOEXCEPT;

	/*
	*	Creates the test scene.
	*/
	void CreateTestScene() NOEXCEPT;

	/*
	*	Generates the terrain.
	*/
	RESTRICTED TerrainEntity *const RESTRICT GenerateTerrain(const Vector3 &worldPosition) NOEXCEPT;

	/*
	*	Generates the vegetation.
	*/
	void GenerateVegetation(const Vector3 &worldPosition, const float extent) NOEXCEPT;

	/*
	*	Generates the particle systems.
	*/
	void GenerateParticleSystems(const Vector3 &worldPosition, const float extent) NOEXCEPT;

};