//Header file.
#include <EngineSystem.h>

//Systems.
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
void EngineSystem::InitializeSystem(const CatalystProjectInformation &initialProjectInformation) NOEXCEPT
{
	//Set the project information.
	projectInformation = initialProjectInformation;

	//Initialize all systems.
	RenderingSystem::Instance->InitializeSystem();
	QuestSystem::Instance->InitializeSystem();

	//Post-initialize all systems.
	InputSystem::Instance->PostInitializeSystem();
	RenderingSystem::Instance->PostInitializeSystem();
}

/*
*	Updates the engine system synchronously. Returns whether or not the game should terminate.
*/
bool EngineSystem::UpdateSystemSynchronous(const float deltaTime) NOEXCEPT
{
	//Update the total game time.
	totalGameTime += deltaTime;

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

	//Return whether or not the game should terminate.
	return shouldTerminate;
}

/*
*	Releases the engine system.
*/
void EngineSystem::ReleaseSystem() NOEXCEPT
{
	//Release all systems.
	InputSystem::Instance->ReleaseSystem();
	EntitySystem::Instance->ReleaseSystem();
	RenderingSystem::Instance->ReleaseSystem();
	QuestSystem::Instance->ReleaseSystem();
}