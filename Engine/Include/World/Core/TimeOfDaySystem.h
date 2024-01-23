#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Entities.
#include <Entities/Types/Entity.h>

//Math.
#include <Math/General/Curve.h>

//World.
#include <World/Sky/SkyGradient.h>
#include <World/TimeOfDay/TimeOfDayParameters.h>

class TimeOfDaySystem final
{

public:

	/*
	*	Terminates the time of day system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Enables the time of day system.
	*/
	void Enable(const TimeOfDayParameters &time_of_day_parameters) NOEXCEPT;

	/*
	*	Returns the current time of day.
	*/
	FORCE_INLINE NO_DISCARD float32 GetCurrentTimeOfDay() const NOEXCEPT
	{
		return _CurrentTimeOfDay;
	}

	/*
	*	Sets the current time of day.
	*/
	FORCE_INLINE void SetCurrentTimeOfDay(const float32 value) NOEXCEPT
	{
		_CurrentTimeOfDay = value;
	}

private:

	//Denotes if the time of day system is enabled.
	bool _Enabled{ false };

	//The current time of day.
	float32 _CurrentTimeOfDay{ 0.0f };

	//The time of day parameters.
	TimeOfDayParameters _TimeOfDayParameters;

	//The sky light curve.
	Curve<Vector4<float32>, 8> _SkyLightCurve;

	//The sky gradient curve.
	Curve<SkyGradient, 4> _SkyGradientCurve;

	//The sky light.
	Entity *RESTRICT _SkyLight;

	/*
	*	Initializes the sky light curve.
	*/
	void InitializeSkyLightCurve() NOEXCEPT;

	/*
	*	Initializes the sky gradient curve.
	*/
	void InitializeSkyGradientCurve() NOEXCEPT;

	/*
	*	Updates the time of day system during the pre update phase.
	*/
	void PreUpdate() NOEXCEPT;

	/*
	*	Updates the sky light.
	*/
	void UpdateSkyLight() NOEXCEPT;

	/*
	*	Updates the sky.
	*/
	void UpdateSky() NOEXCEPT;

};