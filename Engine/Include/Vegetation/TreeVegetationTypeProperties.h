#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class TreeVegetationTypeProperties final
{

public:

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

	//The cutoff distance.
	float _CutoffDistance;

	//The density, expressed as placement per square meter.
	float _Density;

	//The low detail distance.
	float _LowDetailDistance;

	//The medium detail distance.
	float _MediumDetailDistance;

};