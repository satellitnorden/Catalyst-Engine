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
#include <Terrain/TerrainSchedule.h>
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
	*	Returns the low detail terrain patch informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> *const RESTRICT GetTerrainPatchInformations() NOEXCEPT
	{
		return &_PatchInformations;
	}

	/*
	*	Returns the low detail terrain patch render informations.
	*/
	RESTRICTED NO_DISCARD StaticArray<TerrainPatchRenderInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> *const RESTRICT GetTerrainPatchRenderInformations() NOEXCEPT
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

	//Enum covering all terrain system states,
	enum class TerrainSystemState : uint8
	{
		Starting,
		Idling,
		Updating
	};

	//The state.
	TerrainSystemState _State{ TerrainSystemState::Starting };

	//The properties.
	TerrainProperties _Properties;

	//The schedule.
	TerrainSchedule _Schedule;

	//The update.
	TerrainUpdate _Update;

	//The update task.
	Task _UpdateTask;

	//The last grid point.
	GridPoint2 _LastGridPoint{ 0, 0 };

	//The current grid point.
	GridPoint2 _CurrentGridPoint{ 0, 0 };

	//The patch informations.
	StaticArray<TerrainPatchInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> _PatchInformations;

	//The patch render informations.
	StaticArray<TerrainPatchRenderInformation, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> _PatchRenderInformations;

	/*
	*	Processes the update.
	*/
	void ProcessUpdate() NOEXCEPT;

	/*
	*	Updates the terrain system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Follows the schedule.
	*/
	void FollowSchedule() NOEXCEPT;

	/*
	*	Generates a patch.
	*/
	void GeneratePatch(const GridPoint2 &gridPoint, const TerrainBorder borders, const float patchSizeMultiplier, const uint8 normalResolutionMultiplier, TerrainPatchInformation *const RESTRICT patchInformation, TerrainPatchRenderInformation *const RESTRICT patchRenderInformation) NOEXCEPT;


};