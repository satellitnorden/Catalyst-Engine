#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

//Clairvoyant.
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

	/*
	*	Default constructor.
	*/
	WorldArchitect() NOEXCEPT = default;

	/*
	*	Initializes the world architect.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the world architects.
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

	//The generation task.
	Task generationTask;

	//The generation semaphore.
	Semaphore generationSemaphore;

	//The current world architect state.
	WorldArchitectState currentState{ WorldArchitectState::Idling };

	//The world scanning data.
	WorldScanningData scanningData;

	//The world chunks.
	StaticArray<WorldChunk, 9> worldChunks;

	/*
	*	Initializes the generation task.
	*/
	void InitializeGenerationTask() NOEXCEPT;

	/*
	*	Updates the idling state.
	*/
	void UpdateIdling() NOEXCEPT;

	/*
	*	Creates the test scene.
	*/
	void CreateTestScene() NOEXCEPT;

	/*
	*	Generates a new patch.
	*/
	void GeneratePatch(const Vector3 &worldPosition, const uint8 gridPositionX, const uint8 gridPositionY) NOEXCEPT;

	/*
	*	Generates the terrain.
	*/
	void GenerateTerrain(const Vector3 &worldPosition, const uint8 gridPositionX, const uint8 gridPositionY) NOEXCEPT;

	/*
	*	Generates the vegetation.
	*/
	void GenerateVegetation(const Vector3 &worldPosition, const float extent) NOEXCEPT;

	/*
	*	Generates the particle systems.
	*/
	void GenerateParticleSystems(const Vector3 &worldPosition, const float extent) NOEXCEPT;

};