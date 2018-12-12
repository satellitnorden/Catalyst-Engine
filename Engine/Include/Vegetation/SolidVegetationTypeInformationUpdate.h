#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/DynamicArray.h>

//Vegetation.
#include <Vegetation/SolidVegetationTypeInformation.h>

class SolidVegetationTypeInformationUpdate final
{

public:

	//The vegetation type information to update.
	SolidVegetationTypeInformation *RESTRICT _Information;

	//The patch indices to invalidate.
	DynamicArray<uint8> _PatchesToInvalidate;

	//The new patch information.
	VegetationPatchInformation _NewPatchInformation;

	//The new patch render information.
	SolidVegetationPatchRenderInformation _NewPatchRenderInformation;

};