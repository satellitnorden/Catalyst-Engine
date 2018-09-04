#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

class CullingSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(CullingSystem);

	/*
	*	Default constructor.
	*/
	CullingSystem() NOEXCEPT { }

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

	//Container for all tasks.
	StaticArray<Task, INDEX(CullingTask::NumberOfCullingTasks)> _Tasks;

	/*
	*	Initializes the culling tasks.
	*/
	void InitializeCullingTasks() NOEXCEPT;

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