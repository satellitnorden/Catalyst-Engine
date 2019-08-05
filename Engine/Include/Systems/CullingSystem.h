#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	CullingSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the culling system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the culling system during the render update phase.
	*/
	void RenderUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Waits for terrain culling to finish executing.
	*/
	FORCE_INLINE void WaitForTerrainCulling() const NOEXCEPT
	{
		_TerrainCullingTask.WaitFor();
	}

private:

	//The terrain culling task.
	Task _TerrainCullingTask;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() const NOEXCEPT;

};