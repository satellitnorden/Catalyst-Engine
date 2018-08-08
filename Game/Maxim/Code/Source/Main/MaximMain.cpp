//Core.
#include <Core/EngineCore.h>

//Systems.
#include <Systems/EngineSystem.h>

MAIN
{
	//The delta timer.
	DeltaTimer deltaTimer;

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(CatalystProjectInformation(CatalystProjectGeneralInformation("Maxim"),
	CatalystProjectMultithreadingInformation(0),
	CatalystProjectRenderingInformation(),
	CatalystProjectSoundInformation(16)));

	//Main game loop.
	bool shouldTerminate{ false };

	while (!shouldTerminate)
	{
		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the engine system.
		shouldTerminate = EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();

	MAIN_RETURN_SUCCESS;
}