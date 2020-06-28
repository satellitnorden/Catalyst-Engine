#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class SpringDampingSystem final
{

public:

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
	FORCE_INLINE void SetCurrent(const float32 value) NOEXCEPT
	{
		_Current = value;
	}

	/*
	*	Returns the desired value.
	*/
	FORCE_INLINE NO_DISCARD float32 GetDesired() const NOEXCEPT
	{
		return _Desired;
	}

	/*
	*	Sets the desired value.
	*/
	FORCE_INLINE void SetDesired(const float32 desired) NOEXCEPT
	{
		_Desired = desired;
	}

	/*
	*	Sets the damping coefficient.
	*/
	FORCE_INLINE void SetDampingCoefficient(const float32 damping_coefficient) NOEXCEPT
	{
		_DampingCoefficient = damping_coefficient;
	}

	/*
	*	Sets the spring constant.
	*/
	FORCE_INLINE void SetSpringConstant(const float32 spring_constant) NOEXCEPT
	{
		_SpringConstant = spring_constant;
	}

	/*
	*	Updates the spring damping system. Returns the current value.
	*/
	FORCE_INLINE constexpr NO_DISCARD float32 Update(const float32 delta_time) NOEXCEPT
	{
		//Calculate the difference.
		const float32 difference{ _Current - _Desired };

		//Calculate the acceleration.
		const float32 acceleration{ -difference * _SpringConstant - _DampingCoefficient * _Velocity };

		//Update the velocity.
		_Velocity += acceleration * delta_time;

		//Update the current value.
		_Current += _Velocity * delta_time;

		//Return the current value.
		return _Current;
	}

private:

	//The current value.
	float32 _Current{ 0.0f };

	//The desired value.
	float32 _Desired{ 0.0f };

	//The velocity.
	float32 _Velocity{ 0.0f };

	//The damping coefficient.
	float32 _DampingCoefficient{ 0.0f };

	//The spring constant.
	float32 _SpringConstant{ 1.0f };

};