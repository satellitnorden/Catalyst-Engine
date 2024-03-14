#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//World.
#include <World/Level/LevelEntry.h>

class Level final
{

public:

	//The name.
	StaticString<64> _Name;

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

	/*
	*	Serializes this level to the given file path.
	*/
	void Serialize(const char *const RESTRICT file_path) NOEXCEPT;

};