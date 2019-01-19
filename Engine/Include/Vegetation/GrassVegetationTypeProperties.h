#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class GrassVegetationTypeProperties final
{

public:

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

	//The cutoff distance.
	float _CutoffDistance;

	//The density, expressed as placement per square meter.
	float _Density;

	//The thickness.
	float _Thickness;

	//The wind modulator factor.
	float _WindModulatorFactor;

	//The low detail distance.
	float _LowDetailDistance;

	//The medium detail distance.
	float _MediumDetailDistance;

};