#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

class FramePacer final
{

public:

	/*
	*	Initializes the frame pacer.
	*/
	FORCE_INLINE void Initialize() NOEXCEPT
	{
		//Retrieve the next frame value.
		_NextFrame = std::chrono::steady_clock::now();
	}

	/*
	*	Post-updates the frame pacer.
	*/
	FORCE_INLINE void PostUpdate(const float32 desired_refresh_rate) NOEXCEPT
	{
		if (desired_refresh_rate > 0.0f)
		{
			//Update the next frame value.
			_NextFrame += std::chrono::milliseconds(static_cast<uint64>(1.0f / static_cast<float64>(desired_refresh_rate) * 1'000));

			//Sleep until the next frame.
			std::this_thread::sleep_until(_NextFrame);
		}
	}

private:

	//The next frame.
	std::chrono::steady_clock::time_point _NextFrame;

};