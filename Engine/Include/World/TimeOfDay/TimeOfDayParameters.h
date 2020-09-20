#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class TimeOfDayParameters final
{

public:

	//The time multiplier.
	float32 _TimeMultiplier{ 1.0f };

	//Denotes whether or not to control the sky color.
	float32 _ControlSky{ false };

};