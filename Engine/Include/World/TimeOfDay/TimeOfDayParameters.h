#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

class TimeOfDayParameters final
{

public:

	//The save folder.
	DynamicString _SaveFolder;

	//The initial time of day (will load from file if possible).
	float32 _InitialTimeOfDay{ 7.0f };

	//The time multiplier.
	float32 _TimeMultiplier{ 1.0f };

	//Denotes whether or not to control the sky color.
	bool _ControlSky{ false };

};