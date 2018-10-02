#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Spinlock.h>
#include <Multithreading/Task.h>

//Terrain.
#include <Terrain/TerrainPatchInformation.h>
#include <Terrain/TerrainPatchRenderInformation.h>
#include <Terrain/TerrainProperties.h>
#include <Terrain/TerrainUpdate.h>

//Forward declarations.
class CatalystProjectTerrainConfiguration;

class TerrainSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(TerrainSystem);

	/*
	*	Default constructor.
	*/
	TerrainSystem() NOEXCEPT { }

	/*
	*	Initializes the terrain system.
	*/
	void InitializeSystem(const CatalystProjectTerrainConfiguration &configuration) NOEXCEPT;

	/*
	*	Updates the terrain system sequentially.
	*/
	void SequentialUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Returns the terrain properties.
	*/
	RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain patch informations.
	*/
	RESTRICTED NO_DISCARD const StaticArray<TerrainPatchInformation, 9> *const RESTRICT GetTerrainPatchInformations() const NOEXCEPT
	{
		return &_PatchInformations;
	}

	/*
	*	Returns the terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD const StaticArray<TerrainPatchRenderInformation, 9> *const RESTRICT GetTerrainPatchRenderInformations() const NOEXCEPT
	{
		return &_PatchRenderInformations;
	}

	/*
	*	Returns the terrain height at the given position.
	*/
	bool GetTerrainHeightAtPosition(const Vector3 &position, float *const RESTRICT height) const NOEXCEPT;

	/*
	*	Returns the terrain normal at the given position.
	*/
	bool GetTerrainNormalAtPosition(const Vector3 &position, Vector3 *const RESTRICT normal) const NOEXCEPT;

private:

	//The properties.
	TerrainProperties _Properties;

	//The update task.
	Task _UpdateTask;

	//The current camera position.
	Vector3 _CurrentCameraPosition;

	//The patch lock.
	mutable Spinlock _PatchLock;

	//The patch informations.
	StaticArray<TerrainPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<TerrainPatchRenderInformation, 9> _PatchRenderInformations;

	//The terrain update.
	TerrainUpdate _Update;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Generates a new patch at the specified grid point.
	*/
	void GeneratePatch(const GridPoint &gridPoint, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;

	/*
	*	Processes the terrain update.
	*/
	void ProcessTerrainUpdate() NOEXCEPT;

	/*
	*	Invalidates the patch at the specified index.
	*/
	void InvalidatePatch(const uint64 index) NOEXCEPT;

};