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

	//The model.
	PhysicalModel _Model;

	//The material.
	PhysicalMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<VegetationPatchRenderInformation, 9> _PatchRenderInformations;

};