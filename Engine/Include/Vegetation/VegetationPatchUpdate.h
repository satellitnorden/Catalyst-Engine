#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationPatchInformation.h>
#include <Vegetation/VegetationPatchRenderInformation.h>

class VegetationPatchUpdate final
{

public:

	//The patch information.
	VegetationPatchInformation _PatchInformation;

	//The patch render information.
	VegetationPatchRenderInformation _PatchRenderInformation;

};