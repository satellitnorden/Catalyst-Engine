#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Audio.
#include <Audio/Core/Audio.h>

//Math.
#include <Math/Core/BaseMath.h>

/*
*	Class holding a smoothed parameter value, that you can set target value on and it will go towards that target value over time.
*/
class SmoothParameter final
{

public:

	/*
	*	Default constructor.
	*/
	FORCE_INLINE SmoothParameter() NOEXCEPT
		:
		_Current(0.0f),
		_Target(0.0f)
	{
		//Set the speed.
		SetSpeed(10.0f / 1'000.0f);
	}

	/*
	*	Sets the sample rate.
	*/
	FORCE_INLINE void SetSampleRate(const float32 sample_rate) NOEXCEPT
	{
		_SampleRate = sample_rate;
	}

	/*
	*	Returns the speed.
	*/
	FORCE_INLINE NO_DISCARD float32 GetSpeed() const NOEXCEPT
	{
		return _Speed;
	}

	/*
	*	Sets the speed.
	*/
	FORCE_INLINE void SetSpeed(const float32 speed) NOEXCEPT
	{
		_Speed = speed;
		_Alpha = 1.0f - std::exp(-1.0f / (_Speed * _SampleRate));
	}

	/*
	*	Returns the current.
	*/
	FORCE_INLINE NO_DISCARD float32 GetCurrent() const NOEXCEPT
	{
		return _Current;
	}

	/*
	*	Sets the current.
	*/
	FORCE_INLINE void SetCurrent(const float32 current) NOEXCEPT
	{
		_Current = current;
	}

	/*
	*	Returns the target.
	*/
	FORCE_INLINE NO_DISCARD float32 GetTarget() const NOEXCEPT
	{
		return _Target;
	}

	/*
	*	Sets the target.
	*/
	FORCE_INLINE void SetTarget(const float32 target) NOEXCEPT
	{
		_Target = target;
	}

	/*
	*	Updates the current.
	*/
	FORCE_INLINE void UpdateCurrent() NOEXCEPT
	{
		_Current += _Alpha * (_Target - _Current);
	}

private:

	//The sample rate.
	float32 _SampleRate{ Audio::DEFAULT_SAMPLE_RATE };

	//The speed.
	float32 _Speed;

	//The alpha.
	float32 _Alpha;

	//The current.
	float32 _Current;

	//The target.
	float32 _Target;

};