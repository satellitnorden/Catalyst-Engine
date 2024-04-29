#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Systems.
#include <Systems/System.h>

class TimeSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		TimeSystem
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE TimeSystem() NOEXCEPT
	{

	}

};