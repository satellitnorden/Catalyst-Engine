#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Sound.
#include <Sound/SoundCore.h>

class SoundRequest final
{

public:

	//The event description.
	const EventDescription *const RESTRICT eventDescription;

	/*
	*	Constructor taking all values as arguments.
	*/
	SoundRequest(const EventDescription *const RESTRICT initialEventDescription) NOEXCEPT
		:
		eventDescription(initialEventDescription)
	{

	}

};