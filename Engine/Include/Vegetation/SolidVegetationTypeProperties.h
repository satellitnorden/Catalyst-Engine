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

	//The density.
	uint32 _Density;

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

};