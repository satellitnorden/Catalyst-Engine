#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class UpdateContext final
{

public:

	//The total frames.
	uint64 _TotalFrames;

	//The total time.
	float _TotalTime;

	//The delta time.
	float _DeltaTime;

};