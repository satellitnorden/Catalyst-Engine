#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

/*
*	Interpolator class definition.
*/
class Interpolator final
{

public:

	/*
	*	Initializes this interpolator.
	*/
	FORCE_INLINE void Initialize(const float32 initial_maximum_time, const float32 initial_current, const float32 initial_target) NOEXCEPT
	{
		_CurrentTime = 0.0f;
		_MaximumTime = initial_maximum_time;
		_Current = initial_current;
		_Target = initial_target;
	}

	/*
	*	Updates this interpolator.
	*/
	FORCE_INLINE NO_DISCARD float32 Update(const float32 delta_time) NOEXCEPT
	{
		_CurrentTime = CatalystBaseMath::Minimum<float32>(_CurrentTime + delta_time, _MaximumTime);

		return CatalystBaseMath::SmoothStep<1>(CatalystBaseMath::LinearlyInterpolate(_Current, _Target, _CurrentTime / _MaximumTime));
	}

	/*
	*	Sets the current.
	*/
	FORCE_INLINE void SetCurrent(const float32 value) NOEXCEPT
	{
		_Current = value;
	}

	/*
	*	Sets the target.
	*/
	FORCE_INLINE void SetTarget(const float32 value) NOEXCEPT
	{
		_Target = value;
	}

	/*
	*	Updates the target.
	*/
	FORCE_INLINE void UpdateTarget(const float32 new_target) NOEXCEPT
	{
		_CurrentTime = 0.0f;
		_Current = _Target;
		_Target = new_target;
	}

private:

	//The current time.
	float32 _CurrentTime{ 0.0f };

	//The maximum time.
	float32 _MaximumTime;

	//The current.
	float32 _Current;

	//The target.
	float32 _Target;

};