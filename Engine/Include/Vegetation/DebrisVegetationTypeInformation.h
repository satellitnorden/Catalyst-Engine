#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Vegetation.
#include <Vegetation/DebrisVegetationPatchRenderInformation.h>
#include <Vegetation/DebrisVegetationTypeProperties.h>
#include <Vegetation/VegetationPatchInformation.h>

class DebrisVegetationTypeInformation final
{

public:

	//The properties.
	DebrisVegetationTypeProperties _Properties;

	//The model.
	PhysicalModel _Model;

	//The material.
	PhysicalMaterial _Material;

	//The patch informations.
	StaticArray<VegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<DebrisVegetationPatchRenderInformation, 9> _PatchRenderInformations;

};