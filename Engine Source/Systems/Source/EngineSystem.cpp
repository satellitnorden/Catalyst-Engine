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

//Forward declarations.
void UpdateInputSystemAsynchronous() CATALYST_NOEXCEPT;
void UpdateGraphicsSystemAsynchronous() CATALYST_NOEXCEPT;

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
	//Initialize all systems.
	InputSystem::Instance->InitializeSystem();
	EntitySystem::Instance->InitializeSystem();
	GraphicsSystem::Instance->InitializeSystem();
	QuestSystem::Instance->InitializeSystem();
	GAME_SYSTEM_CLASS::Instance->InitializeSystem();

	//Fire off all asynchronous threads.
	asynchronousThreads[AsynchronousThread::InputSystem] = std::thread(&UpdateInputSystemAsynchronous);
	asynchronousThreads[AsynchronousThread::GraphicsSystem] = std::thread(&UpdateGraphicsSystemAsynchronous);
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
	/*CATALYST_BENCHMARK_AVERAGE("GraphicsSystem::UpdateSystemSynchronous", */GraphicsSystem::Instance->UpdateSystemSynchronous()/*)*/;

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
	//Join off all asynchronous threads.
	asynchronousThreads[AsynchronousThread::InputSystem].join();
	asynchronousThreads[AsynchronousThread::GraphicsSystem].join();

	//Release all systems.
	GAME_SYSTEM_CLASS::Instance->ReleaseSystem();
	InputSystem::Instance->ReleaseSystem();
	EntitySystem::Instance->ReleaseSystem();
	GraphicsSystem::Instance->ReleaseSystem();
	QuestSystem::Instance->ReleaseSystem();
}

/*
*	Updates the input system asynchronously.
*/
void UpdateInputSystemAsynchronous() CATALYST_NOEXCEPT
{
	while (!EngineSystem::Instance->ShouldTerminate())
		CATALYST_BENCHMARK_AVERAGE(InputSystem::Instance->UpdateSystemAsynchronous());
}

/*
*	Updates the graphics system asynchronously.
*/
void UpdateGraphicsSystemAsynchronous() CATALYST_NOEXCEPT
{
	while (!EngineSystem::Instance->ShouldTerminate())
		GraphicsSystem::Instance->UpdateSystemAsynchronous();
}