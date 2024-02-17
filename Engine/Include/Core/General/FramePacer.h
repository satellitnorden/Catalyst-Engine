#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/Time.h>

//Concurrency.
#include <Concurrency/ConcurrencyCore.h>

class FramePacer final
{

public:

	/*
	*	Initializes the frame pacer.
	*/
	FORCE_INLINE void Initialize() NOEXCEPT
	{
		//Retrieve the current time point.
		_CurrentTimePoint = GetCurrentTimePoint();
	}

	/*
	*	Post-updates the frame pacer.
	*/
	FORCE_INLINE void PostUpdate(const float32 desired_refresh_rate) NOEXCEPT
	{
		if (desired_refresh_rate > 0.0f)
		{
			//Calculate the refresh rate reciprocal.
			const float32 refresh_rate_reciprocal{ 1.0f / desired_refresh_rate };

			//We can't rely on sleep here, because it's too unreliable.Instead, yield until the next frame.
			while (_CurrentTimePoint.GetSecondsSince() < refresh_rate_reciprocal)
			{
				Concurrency::CurrentThread::Yield();
			}

			//Update the current time point.
			_CurrentTimePoint.AddSeconds(refresh_rate_reciprocal);
		}
	}

private:

	//The current time point.
	TimePoint _CurrentTimePoint;

};