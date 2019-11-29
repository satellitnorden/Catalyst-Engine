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

	/*
	*	Returns the wetness.
	*/
	FORCE_INLINE NO_DISCARD float GetWetness() const NOEXCEPT
	{
		return _Wetness;
	}

	/*
	*	Sets the wetness.
	*/
	FORCE_INLINE void SetWetness(const float wetness) NOEXCEPT
	{
		_Wetness = wetness;
	}

private:

	//The wetness.
	float _Wetness{ 0.0f };

	/*
	*	Updates all particle systems.
	*/
	void UpdateParticleSystems(const UpdateContext* const RESTRICT context) NOEXCEPT;

	/*
	*	Updates all distance triggers.
	*/
	void UpdateDistanceTriggers() NOEXCEPT;

};