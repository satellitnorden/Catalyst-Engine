#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VolumetricLightingProperties final
{

public:

	//Denotes whether or not volumetric lighting is enabled.
	bool _Enabled{ true };

	//The distance.
	float32 _Distance{ 1'024.0f };

	//The height.
	float32 _Height{ 1'024.0f };

	//The thickness.
	float32 _Thickness{ 32.0f };

};