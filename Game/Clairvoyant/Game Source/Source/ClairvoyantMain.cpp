//Engine core.
#include <EngineCore.h>

//Clairvoyant.
#include <ClairvoyantGameSystem.h>

//Systems.
#include <EngineSystem.h>

//STL.
#include <chrono>

//Preprocessor defines.
#define EXIT_SUCCESS 0

//Global variables.
std::chrono::time_point<std::chrono::steady_clock> currentTime{ std::chrono::high_resolution_clock::now() };

/*
*	Returns the delta time.
*/
float GetDeltaTime() NOEXCEPT
{
	//Calculate the new delta time.
	const std::chrono::time_point<std::chrono::steady_clock> newTime{ std::chrono::high_resolution_clock::now() };
	const float deltaTime{ std::chrono::duration<float>(newTime - currentTime).count() };
	currentTime = newTime;

	return deltaTime;
}

int main() NOEXCEPT
{
	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem();

	//Initialize the game system.
	ClairvoyantGameSystem::Instance->InitializeSystem();

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//Calculate the delta time.
		const float deltaTime{ GetDeltaTime() };

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the game system.
		ClairvoyantGameSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the game system.
	ClairvoyantGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	//Return success!
	return EXIT_SUCCESS;
}