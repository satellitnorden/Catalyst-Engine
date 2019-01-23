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

	//The impostor distance.
	float _ImpostorDistance;

	//The low detail distance.
	float _LowDetailDistance;

	//The medium detail distance.
	float _MediumDetailDistance;

	//The impostor width.
	float _ImpostorWidth;

	//The impostor height.
	float _ImpostorHeight;

};