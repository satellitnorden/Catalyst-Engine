#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>
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
	*	Updates the culling sustem during the culling update phase.
	*/
	void CullingUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Waits for the grass vegetation culling to finish.
	*/
	void WaitForGrassVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)].WaitFor();
	}

	/*
	*	Waits for the particle systems culling to finish.
	*/
	void WaitForParticleSystemsCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::ParticleSystems)].WaitFor();
	}

	/*
	*	Waits for the solid vegetation culling to finish.
	*/
	void WaitForSolidVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::SolidVegetation)].WaitFor();
	}

	/*
	*	Waits for the terrain culling to finish.
	*/
	void WaitForTerrainCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::Terrain)].WaitFor();
	}

private:

	//Enumeration covering all culling tasks.
	enum class CullingTask : uint8
	{
		GrassVegetation,
		ParticleSystems,
		SolidVegetation,
		Terrain,

		NumberOfCullingTasks
	};

	//Container for all culling tasks.
	StaticArray<Task, UNDERLYING(CullingTask::NumberOfCullingTasks)> _CullingTasks;

	/*
	*	Culls grass vegetation.
	*/
	void CullGrassVegetation() NOEXCEPT;

	/*
	*	Culls particle systems.
	*/
	void CullParticleSystems() NOEXCEPT;

	/*
	*	Culls solid vegetation.
	*/
	void CullSolidVegetation() NOEXCEPT;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() NOEXCEPT;

};