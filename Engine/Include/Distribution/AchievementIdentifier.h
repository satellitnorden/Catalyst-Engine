#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class AchievementIdentifier final
{

public:

	//The name of the achievement.
	const char* RESTRICT _Name;

	/*
	*	Constructor taking all values as arguments.
	*/
	FORCE_INLINE constexpr AchievementIdentifier(const char *const RESTRICT initial_name) NOEXCEPT
		:
		_Name(initial_name)
	{

	}

};