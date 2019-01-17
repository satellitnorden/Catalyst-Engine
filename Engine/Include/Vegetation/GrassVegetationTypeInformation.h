#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Vegetation.
#include <Vegetation/GrassVegetationMaterial.h>
#include <Vegetation/GrassVegetationModel.h>
#include <Vegetation/GrassVegetationPatchInformation.h>
#include <Vegetation/GrassVegetationPatchRenderInformation.h>
#include <Vegetation/GrassVegetationTypeProperties.h>

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
	StaticArray<GrassVegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<GrassVegetationPatchRenderInformation, 9> _PatchRenderInformations;

};