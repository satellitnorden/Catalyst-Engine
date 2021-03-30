#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class ProceduralGrassProperties final
{

public:

	//The screen coverage. Should be between 0.0f and 1.0f. 1.0f means that one grass blade will spawn on every pixel.
	float32 _ScreenCoverage;

	//The distance between blades.
	float32 _DistanceBetweenBlades;

};