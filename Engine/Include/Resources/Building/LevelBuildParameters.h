#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//World.
#include <World/Level/LevelEntry.h>

class LevelBuildParameters final
{

public:

	//The output file path.
	const char *RESTRICT _OutputFilePath;

	//The identifier.
	const char *RESTRICT _Identifier;

	//The level entries.
	DynamicArray<LevelEntry> _LevelEntries;

};