//Core.
#include <Core/EngineCore.h>

//Main.
#include <Main/MaximGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

/*
*	Creates the Catalyst project configuration.
*/
void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT
{
	configuration->generalConfiguration.projectName = "Maxim";

	configuration->renderingConfiguration.resolution = Resolution(1'920, 1'080);

#if defined(CATALYST_WINDOWS)
	configuration->renderingConfiguration.shadowMapResolution = 2'048;
#else
	configuration->renderingConfiguration.shadowMapResolution = 512;
#endif
}

MAIN_FUNCTION
(
	//The delta timer.
	DeltaTimer deltaTimer;

	//Create the Catalyst project configuration.
	CatalystProjectConfiguration configuration;
	CreateCatalystProjectConfiguration(&configuration);

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(configuration);

	//Initialize the game system.
	MaximGameSystem::Instance->InitializeSystem();

	//Main game loop.
	while (!EngineSystem::Instance->ShouldTerminate())
	{
		CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Game Loop",

		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the game system.
		MaximGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the engine system.
		EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);

		);
	}

	//Release the game system.
	MaximGameSystem::Instance->ReleaseSystem();

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();
)