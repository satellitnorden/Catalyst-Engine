//Header file.
#include <Systems/ComponentSystem.h>

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
	//Initialize all components.
	for (Component *const RESTRICT component : AllComponents())
	{
		component->Initialize();
	}

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
		true
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
		true
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
		true
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
		true
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
		true
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
		true
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
		true
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
		true
	);
}

/*
*	Post-initializes the component system.
*/
void ComponentSystem::PostInitialize() NOEXCEPT
{
	//Post-initialize all components.
	for (Component *const RESTRICT component : AllComponents())
	{
		component->PostInitialize();
	}
}

/*
*	Terminates the component system.
*/
void ComponentSystem::Terminate() NOEXCEPT
{
	//Terminate all components.
	for (Component *const RESTRICT component : AllComponents())
	{
		component->Terminate();
	}
}

/*
*	Updates components for the given update phase.
*/
void ComponentSystem::UpdateComponents(const UpdatePhase update_phase) NOEXCEPT
{
	//Gather the update data.
	_UpdateData.Clear();

	//Run the pre update.
	for (Component *const RESTRICT component : AllComponents())
	{
		ComponentUpdateConfiguration update_configuration;

		component->GetUpdateConfiguration(&update_configuration);

		if (TEST_BIT(update_configuration._UpdatePhaseMask, update_phase))
		{
			component->PreUpdate(update_phase);
		}
	}

	for (Component *const RESTRICT component : AllComponents())
	{
		ComponentUpdateConfiguration update_configuration;

		component->GetUpdateConfiguration(&update_configuration);

		if (TEST_BIT(update_configuration._UpdatePhaseMask, update_phase))
		{
			const uint64 number_of_instances{ component->NumberOfInstances() };

			if (number_of_instances > 0)
			{
				switch (update_configuration._Mode)
				{
					case ComponentUpdateConfiguration::Mode::BATCH:
					{
						for (uint64 batch_start_index{ 0 }; batch_start_index < number_of_instances; batch_start_index += update_configuration._BatchSize)
						{
							_UpdateData.Emplace();
							UpdateData& update_data{ _UpdateData.Back() };

							update_data._Task._Function = [](void *const RESTRICT arguments)
							{
								ComponentSystem::Instance->UpdateComponent(*static_cast<const UpdateData *const RESTRICT>(arguments));
							};
							update_data._Task._Arguments = nullptr; //Will be filled in later.
							update_data._Task._ExecutableOnSameThread = false;
							update_data._Component = component;
							update_data._UpdatePhase = update_phase;
							update_data._StartIndex = batch_start_index;
							update_data._EndIndex = batch_start_index + CatalystBaseMath::Minimum<uint64>(update_configuration._BatchSize, number_of_instances - batch_start_index);
							update_data._SubInstanceIndex = 0;
						}

						break;
					}

					case ComponentUpdateConfiguration::Mode::SUB_INSTANCE:
					{
						for (uint64 instance_index{ 0 }; instance_index < number_of_instances; ++instance_index)
						{
							const uint64 number_of_sub_instances{ component->NumberOfSubInstances(instance_index) };

							for (uint64 sub_instance_index{ 0 }; sub_instance_index < number_of_sub_instances; ++sub_instance_index)
							{
								_UpdateData.Emplace();
								UpdateData& update_data{ _UpdateData.Back() };

								update_data._Task._Function = [](void *const RESTRICT arguments)
								{
									ComponentSystem::Instance->UpdateComponent(*static_cast<const UpdateData *const RESTRICT>(arguments));
								};
								update_data._Task._Arguments = nullptr; //Will be filled in later.
								update_data._Task._ExecutableOnSameThread = false;
								update_data._Component = component;
								update_data._UpdatePhase = update_phase;
								update_data._StartIndex = instance_index;
								update_data._EndIndex = instance_index + 1;
								update_data._SubInstanceIndex = sub_instance_index;
							}
						}

						break;
					}

					default:
					{
						ASSERT(false, "Invalid case!");

						break;
					}
				}
			}
		}
	}

	//Execute all batches!
	for (UpdateData &update_data : _UpdateData)
	{
		update_data._Task._Arguments = &update_data;
		
		TaskSystem::Instance->ExecuteTask(Task::Priority::HIGH, &update_data._Task);
	}

	//Do work until all tasks are finished.
	bool all_done{ _UpdateData.Empty() };

	while (!all_done)
	{
		TaskSystem::Instance->DoWork(Task::Priority::HIGH);

		all_done = true;

		for (UpdateData &update_data : _UpdateData)
		{
			all_done &= update_data._Task.IsExecuted();
		}
	}

	//Run the post update.
	for (Component *const RESTRICT component : AllComponents())
	{
		ComponentUpdateConfiguration update_configuration;

		component->GetUpdateConfiguration(&update_configuration);

		if (TEST_BIT(update_configuration._UpdatePhaseMask, update_phase))
		{
			component->PostUpdate(update_phase);
		}
	}
}

/*
*	Updates a component with the given update data.
*/
void ComponentSystem::UpdateComponent(const UpdateData &update_data) NOEXCEPT
{
	update_data._Component->Update(update_data._UpdatePhase, update_data._StartIndex, update_data._EndIndex, update_data._SubInstanceIndex);
}