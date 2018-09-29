#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationMaterial.h>
#include <Vegetation/VegetationModel.h>
#include <Vegetation/VegetationTypeProperties.h>

class VegetationAddTypeInformation final
{

public:

	//The properties.
	VegetationTypeProperties _Properties;

	//The model.
	VegetationModel _Model;

	//The material.
	VegetationMaterial _Material;

};