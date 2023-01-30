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
	FORCE_INLINE void PostUpdate(const float32 desired_frame_time) NOEXCEPT
	{
		if (desired_frame_time > 0.0f)
		{
			//Update the next frame value.
			_NextFrame += std::chrono::nanoseconds(static_cast<uint64>(static_cast<float64>(desired_frame_time) * 1'000'000'000.0));

			//Sleep until the next frame.
			std::this_thread::sleep_until(_NextFrame);
		}
	}

private:

	//The next frame.
	std::chrono::steady_clock::time_point _NextFrame;

};