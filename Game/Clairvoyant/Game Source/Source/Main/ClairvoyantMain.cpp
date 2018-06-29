//Engine core.
#include <Engine Core/EngineCore.h>

//Clairvoyant.
#include <Main/ClairvoyantGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

//Preprocessor defines.
#define EXIT_SUCCESS 0

int32 main() NOEXCEPT
{
	//The delta timer.
	DeltaTimer deltaTimer;

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(CatalystProjectInformation(	CatalystProjectGeneralInformation("Clairvoyant"),
																			CatalystProjectMultithreadingInformation(8),
																			CatalystProjectRenderingInformation(),
																			CatalystProjectSoundInformation(256)));

	//Initialize the game system.
	ClairvoyantGameSystem::Instance->InitializeSystem();

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the game system.
		ClairvoyantGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the game system.
	ClairvoyantGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	//Return success!
	return EXIT_SUCCESS;
}