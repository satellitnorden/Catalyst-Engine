//Core.
#include <Core/EngineCore.h>

//Main.
#include <Main/MaximGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

MAIN_FUNCTION
(
	//The delta timer.
	DeltaTimer deltaTimer;

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(	CatalystProjectInformation(CatalystProjectGeneralInformation("Maxim"),
												CatalystProjectMultithreadingInformation(0),
												CatalystProjectRenderingInformation(),
												CatalystProjectSoundInformation(16)));

	//Initialize the game system.
	MaximGameSystem::Instance->InitializeSystem();

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the game system.
		MaximGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the game system.
	MaximGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();
)