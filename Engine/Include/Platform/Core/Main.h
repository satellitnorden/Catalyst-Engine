#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

namespace Platform
{

	/*
	*	This defines the main function of the game.
	*	Every platform implements their own respective main functions,
	*	that may do some things before and after calling this function,
	*	but all of them should call this function.
	*/
	void Main() NOEXCEPT;

}
