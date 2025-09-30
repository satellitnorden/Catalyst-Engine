//Header file.
#include <Systems/ComponentSystem.h>

//Components.
#include <Components/Core/Component.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>

/*
*	Initializes the component system.
*/
void ComponentSystem::Initialize() NOEXCEPT
{
	//Initialize components.
	Components::Initialize();
}

/*
*	Post-initializes the component system.
*/
void ComponentSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize components.
	Components::PostInitialize();
}

/*
*	Updates the component system.
*/
void ComponentSystem::Update(const UpdatePhase phase) NOEXCEPT
{
	Components::Update(phase);
}

/*
*	Terminates the component system.
*/
void ComponentSystem::Terminate() NOEXCEPT
{
	//Terminate components.
	Components::Terminate();
}