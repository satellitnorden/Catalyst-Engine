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

#if defined(CATALYST_WINDOWS)
	configuration->_RenderingConfiguration._Resolution = Resolution(1'080 / 2, 1'920 / 2);
	configuration->_RenderingConfiguration._ResolutionScale = 1.0f;
	configuration->_RenderingConfiguration._ShadowMapResolution = 2'048;
#elif defined(CATALYST_ANDROID)
	configuration->_RenderingConfiguration._Resolution = Resolution(1'080, 1'920);
	configuration->_RenderingConfiguration._ResolutionScale = 0.5f;
	configuration->_RenderingConfiguration._ShadowMapResolution = 1'024;
#else
	#error "wat"
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
		//Calculate the delta time.
		const float deltaTime{ deltaTimer.Update() };

		//Update the game system.
		MaximGameSystem::Instance->UpdateSystemSynchronous(deltaTime);

		//Update the engine system.
		EngineSystem::Instance->UpdateSystemSynchronous(deltaTime);
	}

	//Release the engine system.
	EngineSystem::Instance->ReleaseSystem();
)