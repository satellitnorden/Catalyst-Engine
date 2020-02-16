#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//Save.
#include <Save/SaveCore.h>

class SaveEntry final
{

public:

	//The file to the save that will be read from/written to disk.
	DynamicString _File;

	//The save mask. This is used to determine which saves should be saved.
	uint64 _SaveMask;

	//The default values callback. This will be called when no save file was present and default values needs to be filled in.
	DefaultValuesCallback _DefaultValuesCallback;

	//The load callback. This will be called whenever this save is loaded.
	LoadCallback _LoadCallback;

	//The save size callback. This will be called to determine how large the save file should be when saving.
	SaveSizeCallback _SaveSizeCallback;

	//The save callback. This will be called whenever this save is saved.
	SaveCallback _SaveCallback;

};