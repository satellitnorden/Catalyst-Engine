#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

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
	FORCE_INLINE NO_DISCARD float Update() NOEXCEPT
	{
		const std::chrono::time_point<std::chrono::steady_clock> newTime{ std::chrono::high_resolution_clock::now() };
		const float deltaTime{ std::chrono::duration<float>(newTime - _CurrentTime).count() };
		_CurrentTime = newTime;

		return deltaTime;
	}

private:

	//The underlying current time.
	std::chrono::time_point<std::chrono::steady_clock> _CurrentTime{ std::chrono::high_resolution_clock::now() };

};