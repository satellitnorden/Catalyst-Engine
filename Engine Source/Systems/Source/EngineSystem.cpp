//Header file.
#include <EngineSystem.h>

//Systems.
#define GAME_SYSTEM_CLASS_INCLUDE_PATH(CLASS_NAME) <CLASS_NAME.h>
#include GAME_SYSTEM_CLASS_INCLUDE_PATH(GAME_SYSTEM_CLASS)
#include <EntitySystem.h>
#include <GraphicsSystem.h>
#include <InputSystem.h>
#include <PhysicsSystem.h>
#include <QuestSystem.h>

//Singleton definition.
DEFINE_SINGLETON(EngineSystem);

/*
*	Default constructor.
*/
EngineSystem::EngineSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
EngineSystem::~EngineSystem() CATALYST_NOEXCEPT
{

}

/*
*	Initializes the engine system.
*/
void EngineSystem::InitializeSystem() CATALYST_NOEXCEPT
{
	//Initialize the current time.
	currentTime = std::chrono::high_resolution_clock::now();

	//Initialize all systems.
	GraphicsSystem::Instance->InitializeSystem();
	QuestSystem::Instance->InitializeSystem();
	GAME_SYSTEM_CLASS::Instance->InitializeSystem();

	//Post-initialize all systems.
	InputSystem::Instance->PostInitializeSystem();
	GraphicsSystem::Instance->PostInitializeSystem();
}

/*
*	Updates the engine system synchronously. Returns whether or not the game should terminate.
*/
bool EngineSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//CATALYST_BENCHMARK_NAMED_SECTION_AVERAGE("Game loop",

	//Update the game system.
	GAME_SYSTEM_CLASS::Instance->UpdateSystemSynchronous(deltaTime);

	//Pre-update the input system.
	InputSystem::Instance->PreUpdateSystemSynchronous();

	//Pre-update the entity system.
	EntitySystem::Instance->PreUpdateSystemSynchronous();

	//Update the physics system.
	PhysicsSystem::Instance->UpdateSystemSynchronous(deltaTime);

	//Update the graphics system.
	GraphicsSystem::Instance->UpdateSystemSynchronous();

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
void EngineSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	//Release all systems.
	GAME_SYSTEM_CLASS::Instance->ReleaseSystem();
	InputSystem::Instance->ReleaseSystem();
	EntitySystem::Instance->ReleaseSystem();
	GraphicsSystem::Instance->ReleaseSystem();
	QuestSystem::Instance->ReleaseSystem();
}