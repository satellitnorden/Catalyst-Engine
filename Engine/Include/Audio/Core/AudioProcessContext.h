#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class AudioProcessContext final
{

public:

	//Denotes whether or not the timeline was running (on previous process call).
	bool _WasTimelineRunning{ false };

	//Denotes whether or not the timeline is running.
	bool _IsTimelineRunning{ false };

};