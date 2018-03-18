//Engine core.
#include <Engine Core/EngineCore.h>

//Clairvoyant.
#include <ClairvoyantGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

//Preprocessor defines.
#define EXIT_SUCCESS 0

int main() NOEXCEPT
{
	//The delta timer.
	DeltaTimer deltaTimer;

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(CatalystProjectInformation("Clairvoyant", "../../../../../Engine/Engine Source/Shaders/Vulkan/"));

	//Initialize the game system.
	ClairvoyantGameSystem::Instance->InitializeSystem();

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Game Loop",

		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the game system.
		ClairvoyantGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//);
	}

	//Release the game system.
	ClairvoyantGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	//Return success!
	return EXIT_SUCCESS;
}