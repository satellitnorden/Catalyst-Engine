#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Vegetation.
#include <Vegetation/VegetationTypeInformation.h>
#include <Vegetation/VegetationPatchUpdate.h>

class VegetationTypeInformationUpdate final
{

public:

	//The vegetation type information to update.
	VegetationTypeInformation *RESTRICT _Information;

	//The patch indices to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//The patches to be update.
	DynamicArray<VegetationPatchUpdate> _Updates;

};