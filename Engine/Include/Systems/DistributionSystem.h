#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class DistributionSystem final
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

};