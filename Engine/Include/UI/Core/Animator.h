#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Core/BaseMath.h>

namespace UI
{

	/*
	*	Simple animator class.
	*/
	class Animator final
	{

	public:

		/*
		*	Sets the current value.
		*/
		FORCE_INLINE void SetCurrent(const float32 value) NOEXCEPT
		{
			_CurrentValue = value;
		}

		/*
		*	Sets the speed.
		*/
		FORCE_INLINE void SetSpeed(const float32 value) NOEXCEPT
		{
			_Speed = value;
		}

		/*
		*	Updates this animator and returns the current value.
		*/
		FORCE_INLINE NO_DISCARD float32 Update(const float32 delta_time) NOEXCEPT
		{
			_CurrentValue = BaseMath::Minimum<float32>(_CurrentValue + delta_time * _Speed, 1.0f);
			return BaseMath::SmoothStep<1>(_CurrentValue);
		}

	private:

		//The current value.
		float32 _CurrentValue{ 0.0f };

		//The speed.
		float32 _Speed{ 1.0f };

	};
}