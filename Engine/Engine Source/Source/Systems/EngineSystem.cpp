//Header file.
#include <Systems/EngineSystem.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/RenderingSystem.h>
#include <Systems/InputSystem.h>
#include <Systems/PhysicsSystem.h>
#include <Systems/SoundSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EngineSystem);

/*
*	Initializes the engine system.
*/
void EngineSystem::InitializeSystem(const CatalystProjectInformation &initialProjectInformation) NOEXCEPT
{
	//Set the project information.
	projectInformation = initialProjectInformation;

	//Initialize all systems.
	RenderingSystem::Instance->InitializeSystem();
	SoundSystem::Instance->InitializeSystem();
	TaskSystem::Instance->InitializeSystem(projectInformation.multithreadingInformation);
}

/*
*	Updates the engine system synchronously. Returns whether or not the game should terminate.
*/
bool EngineSystem::UpdateSystemSynchronous(const float newDeltaTime) NOEXCEPT
{
	//Update the delta time.
	deltaTime = newDeltaTime;

	//Update the total game time.
	totalGameTime += deltaTime;

	//Pre-update the input system.
	InputSystem::Instance->PreUpdateSystemSynchronous();

	//Update the entity system.
	EntitySystem::Instance->UpdateSystemSynchronous();

	//Update the physics system.
	PhysicsSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Update the graphics system.
	CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Rendering System Update", RenderingSystem::Instance->UpdateSystemSynchronous());

	//Update the sound system.
	SoundSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Post-update the input system.
	InputSystem::Instance->PostUpdateSystemSynchronous();

	//Return whether or not the game should terminate.
	return shouldTerminate;
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