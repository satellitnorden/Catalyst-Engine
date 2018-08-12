//Header file.
#include <Systems/EngineSystem.h>

//Systems.
#include <Systems/CullingSystem.h>
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/LevelOfDetailSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EngineSystem);

/*
*	Initializes the engine system.
*/
void EngineSystem::InitializeSystem(const CatalystProjectConfiguration &initialProjectConfiguration) NOEXCEPT
{
	//Set the project configuration.
	projectConfiguration = initialProjectConfiguration;

	//Initialize all systems.
	TaskSystem::Instance->InitializeSystem(projectConfiguration.multithreadingConfiguration);
	CullingSystem::Instance->InitializeSystem();
	LevelOfDetailSystem::Instance->InitializeSystem();
	RenderingSystem::Instance->InitializeSystem();
	SoundSystem::Instance->InitializeSystem();

	//Post-initialize all systems.
	LevelOfDetailSystem::Instance->PostInitializeSystem();
}

/*
*	Updates the engine system synchronously.
*/
void EngineSystem::UpdateSystemSynchronous(const float newDeltaTime) NOEXCEPT
{
	//Update the delta time.
	deltaTime = newDeltaTime;

	//Update the total game time.
	totalGameTime += deltaTime;

	//Pre-update the input system.
	InputSystem::Instance->PreUpdateSystemSynchronous();

	//Pre-update the entity system.
	EntitySystem::Instance->PreUpdateSystemSynchronous();

	//Pre-update the rendering system.
	RenderingSystem::Instance->PreUpdateSystemSynchronous();

	//Update the culling system.
	CullingSystem::Instance->UpdateSystemSynchronous();

	//Update the level of detail system.
	LevelOfDetailSystem::Instance->UpdateSystemSynchronous();

	//Update the physics system.
	PhysicsSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Update the graphics system.
	RenderingSystem::Instance->UpdateSystemSynchronous();

	//Update the sound system.
	SoundSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Post-update the input system.
	InputSystem::Instance->PostUpdateSystemSynchronous();
}

/*
*	Releases the engine system.
*/
void EngineSystem::ReleaseSystem() NOEXCEPT
{
	//Signal to other systems that the game should terminate.
	shouldTerminate = true;

	//Release all systems.
	InputSystem::Instance->ReleaseSystem();
	EntitySystem::Instance->ReleaseSystem();
	RenderingSystem::Instance->ReleaseSystem();
	SoundSystem::Instance->ReleaseSystem();
	TaskSystem::Instance->ReleaseSystem();
}