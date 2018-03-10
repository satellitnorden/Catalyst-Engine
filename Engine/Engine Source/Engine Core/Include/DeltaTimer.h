#pragma once

//STL.
#include <chrono>

class DeltaTimer final
{

public:

	/*
	*	Updates the current time and returns the delta time.
	*/
	float Update() NOEXCEPT
	{
		const std::chrono::time_point<std::chrono::steady_clock> newTime{ std::chrono::high_resolution_clock::now() };
		const float deltaTime{ std::chrono::duration<float>(newTime - currentTime).count() };
		currentTime = newTime;

		return deltaTime;
	}

private:

	//The underlying current time.
	std::chrono::time_point<std::chrono::steady_clock> currentTime{ std::chrono::high_resolution_clock::now() };

};