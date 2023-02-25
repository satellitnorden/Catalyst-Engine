#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Task.h>

class ALIGN(8) CullingSystem final
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
	void RenderUpdate() NOEXCEPT;

	/*
	*	Waits for dynamic models culling to finish executing.
	*/
	FORCE_INLINE void WaitForDynamicModelsCulling() const NOEXCEPT
	{
		_DynamicModelsCullingTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for instanced static models culling to finish executing.
	*/
	FORCE_INLINE void WaitForInstancedStaticModelsCulling() const NOEXCEPT
	{
		_InstancedStaticModelsCullingTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for static models culling to finish executing.
	*/
	FORCE_INLINE void WaitForStaticModelsCulling() const NOEXCEPT
	{
		_StaticModelsCullingTask.Wait<WaitMode::YIELD>();
	}

	/*
	*	Waits for terrain culling to finish executing.
	*/
	FORCE_INLINE void WaitForTerrainCulling() const NOEXCEPT
	{
		_TerrainCullingTask.Wait<WaitMode::YIELD>();
	}

private:

	//The dynamic models culling task.
	Task _DynamicModelsCullingTask;

	//The instanced static models culling task.
	Task _InstancedStaticModelsCullingTask;

	//The static models culling task.
	Task _StaticModelsCullingTask;

	//The terrain culling task.
	Task _TerrainCullingTask;

	/*
	*	Culls dynamic models.
	*/
	void CullDynamicModels() const NOEXCEPT;

	/*
	*	Culls instanced static models.
	*/
	void CullInstancedStaticModels() const NOEXCEPT;

	/*
	*	Culls static models.
	*/
	void CullStaticModels() const NOEXCEPT;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() const NOEXCEPT;

};