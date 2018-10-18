#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Vegetation.
#include <Vegetation/SolidVegetationTypeProperties.h>
#include <Vegetation/VegetationPatchInformation.h>
#include <Vegetation/VegetationPatchRenderInformation.h>

class SolidVegetationTypeInformation final
{

public:

	//The properties.
	SolidVegetationTypeProperties _Properties;

	//The models.
	StaticArray<PhysicalModel, UNDERLYING(VegetationLevelOfDetail::NumberOfVegetationLevelOfDetails)> _Models;

	//The material.
	PhysicalMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 25> _PatchInformations;

	//The patch render informations.
	StaticArray<VegetationPatchRenderInformation, 25> _PatchRenderInformations;

};