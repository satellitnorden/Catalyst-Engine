#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class VegetationTypeProperties final
{

public:

	//The cutoff distance.
	float _CutoffDistance;

	//The density.
	uint32 _Density;

	//The wind modulator factor.
	float _WindModulatorFactor;

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

};