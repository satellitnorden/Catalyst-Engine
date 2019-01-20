#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Vegetation.
#include <Vegetation/TreeVegetationMaterial.h>
#include <Vegetation/TreeVegetationModel.h>
#include <Vegetation/TreeVegetationPatchRenderInformation.h>
#include <Vegetation/TreeVegetationTypeProperties.h>
#include <Vegetation/VegetationPatchInformation.h>

class TreeVegetationTypeInformation final
{

public:

	//The properties.
	TreeVegetationTypeProperties _Properties;

	//The model.
	TreeVegetationModel _Model;

	//The material.
	TreeVegetationMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<TreeVegetationPatchRenderInformation, 9> _PatchRenderInformations;

};