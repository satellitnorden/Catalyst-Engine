#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class WorldSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(WorldSystem);

	/*
	*	Default constructor.
	*/
	WorldSystem() NOEXCEPT
	{

	}

	/*
	*	Updates the world system during the pre update phase.
	*/
	void PreUpdate(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Updates the world system during the logic update phase.
	*/
	void LogicUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

private:

	/*
	*	Updates all particle systems.
	*/
	void UpdateParticleSystems(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Updates all distance triggers.
	*/
	void UpdateDistanceTriggers() NOEXCEPT;

};