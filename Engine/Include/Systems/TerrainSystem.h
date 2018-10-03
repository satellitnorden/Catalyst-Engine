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
	*	Returns the high detail terrain patch informations.
	*/
	RESTRICTED NO_DISCARD const StaticArray<TerrainPatchInformation, 9> *const RESTRICT GetHighDetailTerrainPatchInformations() const NOEXCEPT
	{
		return &_HighDetailPatchInformations[_CurrentSynchronousBuffer];
	}

	/*
	*	Returns the high detail terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD const StaticArray<TerrainPatchRenderInformation, 9> *const RESTRICT GetHighDetailTerrainPatchRenderInformations() const NOEXCEPT
	{
		return &_HighDetailPatchRenderInformations[_CurrentSynchronousBuffer];
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

	//The update task.
	Task _UpdateTask;

	//The properties.
	TerrainProperties _Properties;

	//The current camera position.
	Vector3 _CurrentCameraPosition;

	//The last grid point.
	GridPoint _LastGridPoint{ INT32_MAXIMUM, INT32_MAXIMUM };

	//The current grid point.
	GridPoint _CurrentGridPoint{ INT32_MAXIMUM, INT32_MAXIMUM };

	//The current synchronous buffer.
	uint8 _CurrentSynchronousBuffer{ 0 };

	//The current asynchronous buffer.
	uint8 _CurrentAsynchronousBuffer{ 1 };

	//The high detail patch informations.
	StaticArray<StaticArray<TerrainPatchInformation, 9>, 2> _HighDetailPatchInformations;

	//The high detail patch render informations.
	StaticArray<StaticArray<TerrainPatchRenderInformation, 9>, 2> _HighDetailPatchRenderInformations;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Generates a new patch at the specified grid point.
	*/
	void GeneratePatch(const GridPoint &gridPoint, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;

};