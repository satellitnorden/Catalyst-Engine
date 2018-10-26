#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Terrain.
#include <Terrain/TerrainCore.h>
#include <Terrain/TerrainScheduleItem.h>

class TerrainSchedule final
{

public:

	/*
	*	Generates a new schedule based on the previous and current grid point.
	*/
	void GenerateNewSchedule(const GridPoint2 previous, const GridPoint2 current) NOEXCEPT;

	/*
	*	Returns if the schedule is done.
	*/
	bool IsDone() const NOEXCEPT
	{
		return _CurrentItem == TerrainConstants::NUMBER_OF_TERRAIN_PATCHES;
	}

	/*
	*	Returns the next schedule item.
	*/
	const TerrainScheduleItem& GetNextScheduleItem() NOEXCEPT
	{
		return _Items[_CurrentItem++];
	}

private:

	//The current item.
	uint8 _CurrentItem;

	//The items.
	StaticArray<TerrainScheduleItem, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> _Items;

	/*
	*	Generates the starting schedule.
	*/
	void GenerateStartingSchedule(const GridPoint2 current) NOEXCEPT;

};