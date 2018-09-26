//Core.
#include <Core/General/DeltaTimer.h>
#include <Core/Core/CatalystCore.h>

//Clairvoyant.
#include <Main/ClairvoyantGameSystem.h>
#if !defined(CATALYST_FINAL)
#include <Main/ClairvoyantResourceCreation.h>
#endif

//Systems.
#include <Systems/EngineSystem.h>

/*
*	Creates the Catalyst project configuration.
*/
void CreateCatalystProjectConfiguration(CatalystProjectConfiguration *const RESTRICT configuration) NOEXCEPT
{
	configuration->_GeneralConfiguration._ProjectName = "Clairvoyant";

	configuration->_RenderingConfiguration._Resolution = Resolution(1'920, 1'080);
	configuration->_RenderingConfiguration._ResolutionScale = 1.0f;
	configuration->_RenderingConfiguration._ShadowMapResolution = 2'048;

	configuration->_TerrainConfiguration._Height = 100.0f;
	configuration->_TerrainConfiguration._TextureTilingFactor = 25.0f;
	configuration->_TerrainConfiguration._PatchResolution = 128;
	configuration->_TerrainConfiguration._PatchSize = 1'000.0f;
}

MAIN_FUNCTION
(
	//Create resources.
	ClairvoyantResourceCreation::CreateResources();

	//Create the Catalyst project configuration.
	CatalystProjectConfiguration configuration;
	CreateCatalystProjectConfiguration(&configuration);

	//Initialize the engine system.
	EngineSystem::Instance->InitializeSystem(configuration);

	//Initialize the Clairvoyant game system.
	ClairvoyantGameSystem::Instance->InitializeSystem();

	//The delta timer.
	DeltaTimer deltaTimer;

	//Main game loop.
	while (!EngineSystem::Instance->ShouldTerminate())
	{
		//CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Game Loop",

		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the engine system.
		EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//);
	}

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();
)