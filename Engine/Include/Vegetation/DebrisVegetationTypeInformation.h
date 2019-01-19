#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Containers/StaticArray.h>

//Rendering.
#include <Rendering/Engine/PhysicalMaterial.h>
#include <Rendering/Engine/PhysicalModel.h>

//Vegetation.
#include <Vegetation/DebrisVegetationPatchInformation.h>
#include <Vegetation/DebrisVegetationPatchRenderInformation.h>
#include <Vegetation/DebrisVegetationTypeProperties.h>

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
	StaticArray<DebrisVegetationPatchInformation, 9> _PatchInformations;

	//The patch render informations.
	StaticArray<DebrisVegetationPatchRenderInformation, 9> _PatchRenderInformations;

};