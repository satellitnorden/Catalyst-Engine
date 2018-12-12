#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Vegetation.
#include <Vegetation/GrassVegetationTypeInformation.h>

class GrassVegetationTypeInformationUpdate final
{

public:

	//The vegetation type information to update.
	GrassVegetationTypeInformation *RESTRICT _Information;

	//The patch indices to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//The new patch information.
	VegetationPatchInformation _NewPatchInformation;

	//The new patch render information.
	GrassVegetationPatchRenderInformation _NewPatchRenderInformation;

};