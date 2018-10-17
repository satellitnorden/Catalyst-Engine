#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Vegetation.
#include <Vegetation/GrassModel.h>
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationTypeProperties.h>
#include <Vegetation/VegetationPatchInformation.h>
#include <Vegetation/VegetationPatchRenderInformation.h>

class GrassVegetationTypeInformation final
{

public:

	//The properties.
	GrassVegetationTypeProperties _Properties;

	//The model.
	StaticArray<GrassModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> _Models;

	//The material.
	GrassVegetationMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 49> _PatchInformations;

	//The patch render informations.
	StaticArray<VegetationPatchRenderInformation, 49> _PatchRenderInformations;

};