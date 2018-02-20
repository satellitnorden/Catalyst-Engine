//Header file.
#include <EngineSystem.h>

//Systems.
#define GAME_SYSTEM_CLASS_INCLUDE_PATH(CLASS_NAME) <CLASS_NAME.h>
#include GAME_SYSTEM_CLASS_INCLUDE_PATH(GAME_SYSTEM_INCLUDE_PATH)
#include <EntitySystem.h>
#include <RenderingSystem.h>
#include <InputSystem.h>
#include <PhysicsSystem.h>
#include <QuestSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EngineSystem);

/*
*	Default constructor.
*/
EngineSystem::EngineSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
EngineSystem::~EngineSystem() NOEXCEPT
{

}

/*
*	Initializes the engine system.
*/
void EngineSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the current time.
	currentTime = std::chrono::high_resolution_clock::now();

	//Initialize all systems.
	RenderingSystem::Instance->InitializeSystem();
	QuestSystem::Instance->InitializeSystem();
	GAME_SYSTEM_CLASS::Instance->InitializeSystem();

	//Post-initialize all systems.
	InputSystem::Instance->PostInitializeSystem();
	RenderingSystem::Instance->PostInitializeSystem();
}

/*
*	Updates the engine system synchronously. Returns whether or not the game should terminate.
*/
bool EngineSystem::UpdateSystemSynchronous() NOEXCEPT
{
	//CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Game loop",

	//Update the game system.
	GAME_SYSTEM_CLASS::Instance->UpdateSystemSynchronous(deltaTime);

	//Pre-update the input system.
	InputSystem::Instance->PreUpdateSystemSynchronous();

	//Update the entity system.
	EntitySystem::Instance->UpdateSystemSynchronous();

	//Update the physics system.
	PhysicsSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Update the graphics system.
	RenderingSystem::Instance->UpdateSystemSynchronous();

	//Post-update the input system.
	InputSystem::Instance->PostUpdateSystemSynchronous();

	//Calculate a new delta time.
	auto newTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(newTime - currentTime).count();
	currentTime = newTime;

	//);

	//Return whether or not the game should terminate.
	return shouldTerminate;
}

/*
*	Releases the engine system.
*/
void EngineSystem::ReleaseSystem() NOEXCEPT
{
	//Release all systems.
	GAME_SYSTEM_CLASS::Instance->ReleaseSystem();
	InputSystem::Instance->ReleaseSystem();
	EntitySystem::Instance->ReleaseSystem();
	RenderingSystem::Instance->ReleaseSystem();
	QuestSystem::Instance->ReleaseSystem();
}