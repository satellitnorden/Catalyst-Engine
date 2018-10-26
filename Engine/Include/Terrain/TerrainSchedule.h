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
	*	Generates a new schedule based on the delta.
	*/
	void GenerateNewSchedule(const GridPoint2 delta) NOEXCEPT;

private:

	//The items.
	StaticArray<TerrainScheduleItem, TerrainConstants::NUMBER_OF_TERRAIN_PATCHES> _Items;

};