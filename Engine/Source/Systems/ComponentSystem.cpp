//Header file.
#include <Systems/ComponentSystem.h>

//Components.
#include <Components/Core/Component.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(ComponentSystem);

/*
*	Initializes the component system.
*/
void ComponentSystem::Initialize() NOEXCEPT
{
	//Initialize components.
	Components::Initialize();

	//Register all updates.
	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::PRE);
		},
		this,
		UpdatePhase::PRE,
		UpdatePhase::ENTITY,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::INPUT);
		},
		this,
		UpdatePhase::INPUT,
		UpdatePhase::GAMEPLAY,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::GAMEPLAY);
		},
		this,
		UpdatePhase::GAMEPLAY,
		UpdatePhase::USER_INTERFACE,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::USER_INTERFACE);
		},
		this,
		UpdatePhase::USER_INTERFACE,
		UpdatePhase::PHYSICS,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::PHYSICS);
		},
		this,
		UpdatePhase::PHYSICS,
		UpdatePhase::PRE_RENDER,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::PRE_RENDER);
		},
		this,
		UpdatePhase::PRE_RENDER,
		UpdatePhase::RENDER,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::RENDER);
		},
		this,
		UpdatePhase::RENDER,
		UpdatePhase::POST,
		true,
		false
	);

	CatalystEngineSystem::Instance->RegisterUpdate
	(
		[](void *const RESTRICT arguments)
		{
			Components::Update(UpdatePhase::POST);
		},
		this,
		UpdatePhase::POST,
		UpdatePhase::PRE,
		true,
		false
	);
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
*	Terminates the component system.
*/
void ComponentSystem::Terminate() NOEXCEPT
{
	//Terminate components.
	Components::Terminate();
}