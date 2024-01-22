#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Task.h>

class CullingSystem final
{

public:

	/*
	*	Initializes the culling system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the culling system during the pre render update phase.
	*/
	void PreRenderUpdate() NOEXCEPT;

	/*
	*	Waits for terrain culling to finish executing.
	*/
	FORCE_INLINE void WaitForTerrainCulling() const NOEXCEPT
	{
		_TerrainCullingTask.Wait<WaitMode::YIELD>();
	}

private:

	//The terrain culling task.
	Task _TerrainCullingTask;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() const NOEXCEPT;

};