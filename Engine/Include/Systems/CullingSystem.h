#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>
#include <Core/General/UpdateContext.h>

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
	void UpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Waits for the debris vegetation culling to finish.
	*/
	void WaitForDebrisVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::DebrisVegetation)].WaitFor();
	}

	/*
	*	Waits for the grass vegetation culling to finish.
	*/
	void WaitForGrassVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::GrassVegetation)].WaitFor();
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

	/*
	*	Waits for the tree vegetation culling to finish.
	*/
	void WaitForTreeVegetationCulling() const NOEXCEPT
	{
		_CullingTasks[UNDERLYING(CullingTask::TreeVegetation)].WaitFor();
	}

private:

	//Enumeration covering all culling tasks.
	enum class CullingTask : uint8
	{
		DebrisVegetation,
		GrassVegetation,
		SolidVegetation,
		Terrain,
		TreeVegetation,

		NumberOfCullingTasks
	};

	//Container for all culling tasks.
	StaticArray<Task, UNDERLYING(CullingTask::NumberOfCullingTasks)> _CullingTasks;

	/*
	*	Culls debris vegetation.
	*/
	void CullDebrisVegetation() NOEXCEPT;

	/*
	*	Culls grass vegetation.
	*/
	void CullGrassVegetation() NOEXCEPT;

	/*
	*	Culls solid vegetation.
	*/
	void CullSolidVegetation() NOEXCEPT;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() NOEXCEPT;

	/*
	*	Culls tre vegetation.
	*/
	void CullTreeVegetation() NOEXCEPT;

};