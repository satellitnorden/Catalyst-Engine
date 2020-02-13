#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Entities.
#include <Entities/Types/LightEntity.h>

//World.
#include <World/TimeOfDay/TimeOfDayParameters.h>

class TimeOfDaySystem final
{

public:

	/*
	*	Updates the time of day system during the pre update phase.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Enables the time of day system.
	*/
	void Enable(const float time_of_day, const TimeOfDayParameters &time_of_day_parameters) NOEXCEPT;

	/*
	*	Returns the current time of day.
	*/
	FORCE_INLINE NO_DISCARD float GetCurrentTimeOfDay() const NOEXCEPT
	{
		return _CurrentTimeOfDay;
	}

private:

	//Denotes if the time of day system is enabled.
	bool _Enabled{ false };

	//The current time of day.
	float _CurrentTimeOfDay{ 0.0f };

	//The time of day parameters.
	TimeOfDayParameters _TimeOfDayParameters;

	//The sky light.
	LightEntity *RESTRICT _SkyLight;

	/*
	*	Updates the sky light.
	*/
	void UpdateSkyLight() NOEXCEPT;

};