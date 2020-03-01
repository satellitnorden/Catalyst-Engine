#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class VolumetricLightingProperties final
{

public:

	//The distance.
	float32 _Distance{ 2'048.0f };

	//The height.
	float32 _Height{ 1'024.0f };

	//The thickness.
	float32 _Thickness{ 1.0f };

};