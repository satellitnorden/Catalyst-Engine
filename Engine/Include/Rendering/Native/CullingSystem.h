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
	*	Waits for instanced impostors culling to finish executing.
	*/
	FORCE_INLINE void WaitForInstancedImpostorsCulling() const NOEXCEPT
	{
		_InstancedImpostorsCullingTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for instanced static models culling to finish executing.
	*/
	FORCE_INLINE void WaitForInstancedStaticModelsCulling() const NOEXCEPT
	{
		_InstancedStaticModelsCullingTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for terrain culling to finish executing.
	*/
	FORCE_INLINE void WaitForTerrainCulling() const NOEXCEPT
	{
		_TerrainCullingTask.Wait<WaitMode::YIELD>();
	}

private:

	//The instanced static models culling task.
	Task _InstancedImpostorsCullingTask;

	//The instanced impostors culling task.
	Task _InstancedStaticModelsCullingTask;

	//The terrain culling task.
	Task _TerrainCullingTask;

	/*
	*	Culls instanced impostors.
	*/
	void CullInstancedImpostors() const NOEXCEPT;

	/*
	*	Culls instanced static models.
	*/
	void CullInstancedStaticModels() const NOEXCEPT;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() const NOEXCEPT;

};