#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Vegetation.
#include <Vegetation/GrassVegetationModel.h>
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
	GrassVegetationModel _Model;

	//The material.
	GrassVegetationMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 25> _PatchInformations;

	//The patch render informations.
	StaticArray<VegetationPatchRenderInformation, 25> _PatchRenderInformations;

};