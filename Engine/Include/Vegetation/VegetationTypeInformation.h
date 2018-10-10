#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Vegetation.
#include <Vegetation/GrassMaterial.h>
#include <Vegetation/GrassModel.h>
#include <Vegetation/VegetationPatchInformation.h>
#include <Vegetation/VegetationPatchRenderInformation.h>
#include <Vegetation/VegetationTypeProperties.h>

class VegetationTypeInformation final
{

public:

	//The properties.
	VegetationTypeProperties _Properties;

	//The model.
	GrassModel _Model;

	//The material.
	GrassMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<VegetationPatchRenderInformation, 9> _PatchRenderInformations;

};