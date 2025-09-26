#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VolumetricLightingProperties final
{

public:

	//Denotes whether or not volumetric lighting is enabled.
	bool _Enabled{ true };

	//The density.
	float32 _Density{ 0.1f };

	//The height.
	float32 _Height{ 256.0f };

};