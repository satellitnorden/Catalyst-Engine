#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class GrassVegetationTypeProperties final
{

public:

	//The cutoff distance.
	float _CutoffDistance;

	//The density, expressed as placement per square meter.
	float _Density;

	//The thickness.
	float _Thickness;

	//The wind modulator factor.
	float _WindModulatorFactor;

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

};