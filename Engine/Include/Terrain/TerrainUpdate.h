#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

class TerrainUpdate final
{

public:

	//The patches to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//Denotes whether or not there is a new patch.
	bool _AddPatch;

	//The new patch information.
	TerrainPatchInformation _NewPatchInformation;

	//The new patch render information.
	TerrainPatchRenderInformation _NewPatchRenderInformation;

};