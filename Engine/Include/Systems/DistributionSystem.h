#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Distribution.
#include <Distribution/AchievementIdentifier.h>

class ALIGN(8) DistributionSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(DistributionSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE DistributionSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the distribution system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the distribution system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Returns the current user name.
	*/
	RESTRICTED NO_DISCARD const char *const RESTRICT CurrentUserName() const NOEXCEPT;

	/*
	*	Clears an achievement, resetting it's state.
	*/
	void ClearAchievement(const AchievementIdentifier &identifier) const NOEXCEPT;

	/*
	*	Grants an achievement.
	*/
	void GrantAchievement(const AchievementIdentifier &identifier) const NOEXCEPT;

};