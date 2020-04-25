#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/CatalystBaseMath.h>

class SoundMixComponent final
{

public:

	//Enumerations covering all types.
	enum class Type : uint8
	{
		EXPONENTIALLY_MOVING_AVERAGE_FILTER,
		LIMITER
	};

	/*
	*	Creates an exponentially moving average filter sound mix component.
	*/
	FORCE_INLINE static NO_DISCARD SoundMixComponent CreateExponentiallyMovingAverageFilter(const float32 initial_feedback_factor) NOEXCEPT
	{
		SoundMixComponent new_component;

		new_component._Type = Type::EXPONENTIALLY_MOVING_AVERAGE_FILTER;

		new_component._ExponentiallyMovingAverageFilterState._FeedbackFactor = initial_feedback_factor;
		new_component._ExponentiallyMovingAverageFilterState._CurrentValue = 0.0f;

		return new_component;
	}

	/*
	*	Creates a limiter sound mix component.
	*/
	FORCE_INLINE static NO_DISCARD SoundMixComponent CreateLimiter(const float32 initial_boost, const float32 initial_ceiling) NOEXCEPT
	{
		SoundMixComponent new_component;

		new_component._Type = Type::LIMITER;

		new_component._LimiterState._Boost = initial_boost;
		new_component._LimiterState._Ceiling = initial_ceiling;

		return new_component;
	}

	/*
	*	The process function.
	*/
	FORCE_INLINE void Process(float32 *const RESTRICT sample) NOEXCEPT
	{
		//Different processing depending on type.
		switch (_Type)
		{
			case Type::EXPONENTIALLY_MOVING_AVERAGE_FILTER:
			{
				ProcessExponentiallyMovingAverageFilter(sample);

				break;
			}

			case Type::LIMITER:
			{
				ProcessLimiter(sample);

				break;
			}

			default:
			{
				ASSERT(false, "Invalid case!");

				break;
			}
		}
	}

private:

	/*
	*	Exponentially moving average filter state.
	*/
	class ExponentiallyMovingAverageFilterState final
	{
	
	public:

		//The feedback factor.
		float32 _FeedbackFactor;

		//The current value.
		float32 _CurrentValue;
	
	};

	/*
	*	Limiter state.
	*/
	class LimiterState final
	{

	public:

		//The boost
		float32 _Boost;

		//The ceiling
		float32 _Ceiling;

	};

	//The type.
	Type _Type;

	//The state.
	union
	{
		ExponentiallyMovingAverageFilterState _ExponentiallyMovingAverageFilterState;
		LimiterState _LimiterState;
	};

	/*
	*	The process function for the exponentially moving average filer.
	*/
	FORCE_INLINE void ProcessExponentiallyMovingAverageFilter(float32 *const RESTRICT sample) NOEXCEPT
	{
		_ExponentiallyMovingAverageFilterState._CurrentValue = CatalystBaseMath::LinearlyInterpolate(*sample, _ExponentiallyMovingAverageFilterState._CurrentValue, _ExponentiallyMovingAverageFilterState._FeedbackFactor);

		*sample = _ExponentiallyMovingAverageFilterState._CurrentValue;
	}

	/*
	*	The process function for the limiter.
	*/
	FORCE_INLINE void ProcessLimiter(float32 *const RESTRICT sample) NOEXCEPT
	{
		*sample = CatalystBaseMath::Clamp(*sample * _LimiterState._Boost, -_LimiterState._Ceiling, _LimiterState._Ceiling);
	}

};