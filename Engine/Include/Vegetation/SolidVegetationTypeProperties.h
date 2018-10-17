#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Vegetation.
#include <Vegetation/VegetationCore.h>

class SolidVegetationTypeProperties final
{

public:

	//The distance beyond which the solid vegetation type is rendered as medium detail.
	float _MediumDetailDistance;

	//The distance beyond which the solid vegetation type is rendered as low detail.
	float _LowDetailDistance;

	//The cutoff distance.
	float _CutoffDistance;

	//The density.
	uint32 _Density;

	//The placement function.
	VegetationPlacementFunction _PlacementFunction;

};