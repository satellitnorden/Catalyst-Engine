#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

//Resources.
#include <Resources/Core/ResourcePointer.h>

//World.
#include <World/Core/EnvironmentSystem.h>
#include <World/Core/SkySystem.h>
#include <World/Core/TimeOfDaySystem.h>

//Forward declarations.
class LevelResource;

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
	*	Initializes the world system.
	*/
	void Initialize(const CatalystProjectWorldConfiguration& configuration) NOEXCEPT;

	/*
	*	Post-initializes the world system.
	*/
	void PostInitialize() NOEXCEPT;

	/*
	*	Returns the world grid size.
	*/
	FORCE_INLINE NO_DISCARD float32 GetWorldGridSize() const NOEXCEPT
	{
		return _WorldGridSize;
	}

	/*
	*	Returns the current world grid cell.
	*/
	NO_DISCARD const Vector3<int32>& GetCurrentWorldGridCell() const NOEXCEPT;

	/*
	*	Returns the environment system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD EnvironmentSystem *const RESTRICT GetEnvironmentSystem() NOEXCEPT
	{
		return &_EnvironmentSystem;
	}

	/*
	*	Returns the sky system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD SkySystem *const RESTRICT GetSkySystem() NOEXCEPT
	{
		return &_SkySystem;
	}

	/*
	*	Returns the time of day system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD TimeOfDaySystem *const RESTRICT GetTimeOfDaySystem() NOEXCEPT
	{
		return &_TimeOfDaySystem;
	}

	/*
	*	Loads a level.
	*/
	void LoadLevel(const ResourcePointer<LevelResource> resource) NOEXCEPT;

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

	//The world grid size.
	float32 _WorldGridSize;

	//The environment system.
	EnvironmentSystem _EnvironmentSystem;

	//The sky system.
	SkySystem _SkySystem;

	//The time of day system.
	TimeOfDaySystem _TimeOfDaySystem;

	//The wetness.
	float _Wetness{ 0.0f };

	/*
	*	Updates the world system during the input update phase.
	*/
	void InputUpdate() NOEXCEPT;

	/*
	*	Updates the world system during the logic update phase.
	*/
	void LogicUpdate() NOEXCEPT;

	/*
	*	Updates all particle systems.
	*/
	void UpdateParticleSystems() NOEXCEPT;

	/*
	*	Updates all distance triggers.
	*/
	void UpdateDistanceTriggers() NOEXCEPT;

};