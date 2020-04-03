#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Concurrency.
#include <Concurrency/Task.h>

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

	/*
	*	Waits for vegetation culling to finish executing.
	*/
	FORCE_INLINE void WaitForVegetationCulling() const NOEXCEPT
	{
		_VegetationCullingTask.WaitFor();
	}

private:

	//The terrain culling task.
	Task _TerrainCullingTask;

	//The vegetation culling task.
	Task _VegetationCullingTask;

	/*
	*	Culls terrain.
	*/
	void CullTerrain() const NOEXCEPT;

	/*
	*	Culls vegetation.
	*/
	void CullVegetation() const NOEXCEPT;

};