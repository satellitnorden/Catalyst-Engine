//Core.
#include <Core/General/DeltaTimer.h>
#include <Core/Core/CatalystCore.h>

//Main.
#include <Main/MaximGameSystem.h>

//Systems.
#include <Systems/EngineSystem.h>

/*
*	Creates the Catalyst project configuration.
*/
void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT
{
	configuration->_GeneralConfiguration._ProjectName = "Maxim";

	configuration->_RenderingConfiguration._Resolution = Resolution(1'920, 1'080);

#if defined(CATALYST_WINDOWS)
	configuration->_RenderingConfiguration._ResolutionScale = 1.0f;
	configuration->_RenderingConfiguration._ShadowMapResolution = 2'048;
#else
	configuration->_RenderingConfiguration._ResolutionScale = 0.75f;
	configuration->_RenderingConfiguration._ShadowMapResolution = 1'024;
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