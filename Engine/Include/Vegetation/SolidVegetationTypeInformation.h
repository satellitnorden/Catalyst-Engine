#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Vegetation.
#include <Vegetation/SolidVegetationPatchRenderInformation.h>
#include <Vegetation/SolidVegetationTypeProperties.h>
#include <Vegetation/VegetationPatchInformation.h>

class SolidVegetationTypeInformation final
{

public:

	//The properties.
	SolidVegetationTypeProperties _Properties;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<SolidVegetationPatchRenderInformation, 9> _PatchRenderInformations;

};