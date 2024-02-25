#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>
#include <Core/General/Padding.h>

//World.
#include <World/Core/EnvironmentSystem.h>
#include <World/Core/SkySystem.h>
#include <World/Core/TimeOfDaySystem.h>
#include <World/Core/WaterSystem.h>
#include <World/Core/WindSystem.h>

//Forward declarations.
class LevelResource;

class ALIGN(8) WorldSystem final
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
	*	Terminates the world system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Updates the world system during render.
	*/
	void RenderUpdate() NOEXCEPT;

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
	*	Returns the water system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD WaterSystem *const RESTRICT GetWaterSystem() NOEXCEPT
	{
		return &_WaterSystem;
	}

	/*
	*	Returns the wind system.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD WindSystem *const RESTRICT GetWindSystem() NOEXCEPT
	{
		return &_WindSystem;
	}

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

	/*
	*	World uniform data class definition.
	*/
	class WorldUniformData final
	{

	public:

		//The upper sky gradient.
		Vector3<float32> _UpperSkyColor;

		//Some padding.
		Padding<4> _Padding1;

		//The lower sky gradient.
		Vector3<float32> _LowerSkyColor;

		//Some padding.
		Padding<4> _Padding2;

	};

	//The world grid size.
	float32 _WorldGridSize;

	//The environment system.
	EnvironmentSystem _EnvironmentSystem;

	//The sky system.
	SkySystem _SkySystem;

	//The time of day system.
	TimeOfDaySystem _TimeOfDaySystem;

	//The water system.
	WaterSystem _WaterSystem;

	//The wind system.
	WindSystem _WindSystem;

	//The wetness.
	float _Wetness{ 0.0f };

	//The world uniform data.
	WorldUniformData _WorldUniformData;

};