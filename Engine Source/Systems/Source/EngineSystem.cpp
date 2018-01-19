//Header file.
#include <EngineSystem.h>

//Systems.
#define GAME_SYSTEM_CLASS_INCLUDE_PATH(CLASS_NAME) <CLASS_NAME.h>
#include GAME_SYSTEM_CLASS_INCLUDE_PATH(GAME_SYSTEM_CLASS)
#include <EntitySystem.h>
#include <GraphicsSystem.h>
#include <InputSystem.h>
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
	//Pre-initialize all systems.
	InputSystem::Instance->PreInitializeSystem();
	EntitySystem::Instance->InitializeSystem();
	GraphicsSystem::Instance->PreInitializeSystem();
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
	//Update the game system.
	GAME_SYSTEM_CLASS::Instance->UpdateSystemSynchronous(deltaTime);

	//Update the input system.
	InputSystem::Instance->UpdateSystemSynchronous();

	//Update the entity system.
	EntitySystem::Instance->UpdateSystemSynchronous();

	//Update the graphics system.
	GraphicsSystem::Instance->UpdateSystemSynchronous();

	//Update the quest system.
	QuestSystem::Instance->UpdateSystemSynchronous();

	//Calculate a new delta time.
	auto newTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float>(newTime - currentTime).count();
	currentTime = newTime;

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