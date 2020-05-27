#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//Resources.
#include <Resources/Core/Resource.h>

//World.
#include <World/Level/LevelEntry.h>

class LevelResource final : public Resource
{

public:

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

};