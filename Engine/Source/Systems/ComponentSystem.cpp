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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::PRE);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::INPUT);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::GAMEPLAY);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::USER_INTERFACE);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::PHYSICS);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::PRE_RENDER);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::RENDER);
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
			static_cast<ComponentSystem *const RESTRICT>(arguments)->UpdateComponents(UpdatePhase::POST);
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

/*
*	Updates components for the given update phase.
*/
void ComponentSystem::UpdateComponents(const UpdatePhase update_phase) NOEXCEPT
{
	//Update components.
	Components::Update(update_phase);

	//Run the post update.
	for (uint64 component_index{ 0 }; component_index < Components::Size(); ++component_index)
	{
		//Cache the component.
		Component *const RESTRICT component{ Components::At(component_index) };

		ComponentUpdateConfiguration update_configuration;

		component->GetUpdateConfiguration(&update_configuration);

		if (TEST_BIT(update_configuration._UpdatePhaseMask, update_phase))
		{
			component->PostUpdate(update_phase);
		}
	}
}