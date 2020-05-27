#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//World.
#include <World/Level/LevelEntry.h>

class LevelData final
{

public:

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

};