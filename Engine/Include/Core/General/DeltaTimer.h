#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

template <typename TYPE>
class DeltaTimer final
{

public:

	/*
	*	Resets this delta timer.
	*/
	FORCE_INLINE void Reset() NOEXCEPT
	{
		_CurrentTime = std::chrono::high_resolution_clock::now();
	}

	/*
	*	Updates the current time and returns the delta time.
	*/
	FORCE_INLINE NO_DISCARD TYPE Update() NOEXCEPT
	{
		const std::chrono::time_point<std::chrono::steady_clock> new_time{ std::chrono::high_resolution_clock::now() };
		const TYPE delta_time{ std::chrono::duration<TYPE>(new_time - _CurrentTime).count() };
		_CurrentTime = new_time;

		return delta_time;
	}

private:

	//The underlying current time.
	std::chrono::time_point<std::chrono::steady_clock> _CurrentTime{ std::chrono::high_resolution_clock::now() };

};