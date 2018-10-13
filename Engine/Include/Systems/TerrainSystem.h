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
	*	Returns the terrain properties, const.
	*/
	RESTRICTED NO_DISCARD const TerrainProperties *const RESTRICT GetTerrainProperties() const NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the terrain properties, non-const.
	*/
	RESTRICTED NO_DISCARD TerrainProperties *const RESTRICT GetTerrainProperties() NOEXCEPT
	{
		return &_Properties;
	}

	/*
	*	Returns the high detail terrain patch informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchInformation, 9> *const RESTRICT GetHighDetailTerrainPatchInformations() NOEXCEPT
	{
		return &_HighDetailPatchInformations[_CurrentSynchronousBuffer];
	}

	/*
	*	Returns the high detail terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchRenderInformation, 9> *const RESTRICT GetHighDetailTerrainPatchRenderInformations() NOEXCEPT
	{
		return &_HighDetailPatchRenderInformations[_CurrentSynchronousBuffer];
	}

	/*
	*	Returns the low detail terrain patch informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchInformation, 56> *const RESTRICT GetLowDetailTerrainPatchInformations() NOEXCEPT
	{
		return &_LowDetailPatchInformations[_CurrentSynchronousBuffer];
	}

	/*
	*	Returns the low detail terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchRenderInformation, 56> *const RESTRICT GetLowDetailTerrainPatchRenderInformations() NOEXCEPT
	{
		return &_LowDetailPatchRenderInformations[_CurrentSynchronousBuffer];
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
	GridPoint2 _LastGridPoint{ INT32_MAXIMUM, INT32_MAXIMUM };

	//The current grid point.
	GridPoint2 _CurrentGridPoint{ INT32_MAXIMUM, INT32_MAXIMUM };

	//The current synchronous buffer.
	uint8 _CurrentSynchronousBuffer{ 0 };

	//The current asynchronous buffer.
	uint8 _CurrentAsynchronousBuffer{ 1 };

	//The high detail patch informations.
	StaticArray<StaticArray<TerrainPatchInformation, 9>, 2> _HighDetailPatchInformations;

	//The high detail patch render informations.
	StaticArray<StaticArray<TerrainPatchRenderInformation, 9>, 2> _HighDetailPatchRenderInformations;

	//The low detail patch informations.
	StaticArray<StaticArray<TerrainPatchInformation, 56>, 2> _LowDetailPatchInformations;

	//The low detail patch render informations.
	StaticArray<StaticArray<TerrainPatchRenderInformation, 56>, 2> _LowDetailPatchRenderInformations;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Generates low detail patches.
	*/
	void GenerateLowDetailPatches(const GridPoint2 &currentGridPoint, const int32 gridPointOffset, const uint8 layer) NOEXCEPT;

	/*
	*	Generates a new high detail patch at the specified grid point.
	*/
	void GenerateHighDetailPatch(const GridPoint2 &gridPoint, const TerrainAxis borders, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;

	/*
	*	Generates a new low detail patch at the specified grid point.
	*/
	void GenerateLowDetailPatch(const GridPoint2 &gridPoint, const TerrainAxis borders, const float patchSizeMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;


};