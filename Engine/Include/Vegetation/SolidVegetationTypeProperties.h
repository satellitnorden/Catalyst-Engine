#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class SolidVegetationTypeProperties final
{

public:

	//The cutoff distance.
	float _CutoffDistance;

	//The density, expressed as placement per square meter.
	float _Density;

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

};