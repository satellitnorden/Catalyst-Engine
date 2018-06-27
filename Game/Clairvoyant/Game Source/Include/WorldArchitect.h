#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/Task.h>

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

	//The generation task.
	Task generationTask;

	//The generation semaphore.
	Semaphore generationSemaphore;

	/*
	*	Initializes the generation task.
	*/
	void InitializeGenerationTask() NOEXCEPT;

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