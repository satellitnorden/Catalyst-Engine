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
	*	Given an X and Y coordinate in the 0.0f-1.0f range, calculate and return the island falloff multiplier.
	*/
	float CalculateIslandFalloffMultiplier(const float xCoordinate, const float yCoordinate) NOEXCEPT;

	/*
	*	Generates a new island.
	*/
	void GenerateIsland(const Vector3 &worldPosition) NOEXCEPT;

	/*
	*	Generates the terrain.
	*/
	void GenerateTerrain(const Vector3 &worldPosition, float& extent) NOEXCEPT;

	/*
	*	Generates the vegetation.
	*/
	void GenerateVegetation(const Vector3 &worldPosition, const float extent) NOEXCEPT;

	/*
	*	Generates the particle systems.
	*/
	void GenerateParticleSystems(const Vector3 &worldPosition, const float extent) NOEXCEPT;

};