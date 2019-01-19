#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Vegetation.
#include <Vegetation/DebrisVegetationTypeInformation.h>

class DebrisVegetationTypeInformationUpdate final
{

public:

	//The vegetation type information to update.
	DebrisVegetationTypeInformation *RESTRICT _Information;

	//Denotes whether or not this is a level of detail update.
	bool _LevelOfDetailUpdate;

	//The index.
	uint8 _Index;

	//The patch indices to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//The new patch information.
	DebrisVegetationPatchInformation _NewPatchInformation;

	//The new patch render information.
	DebrisVegetationPatchRenderInformation _NewPatchRenderInformation;

};