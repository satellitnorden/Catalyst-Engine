//Header file.
#include <Components/Components/ScriptComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Generated
#include <Generated/Script.Generated.h>

DEFINE_COMPONENT(ScriptComponent, ScriptInitializationData, ScriptInstanceData);

/*
*	Initializes this component.
*/
void ScriptComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableEnumerationField
	(
		"Script",
		offsetof(ScriptInitializationData, _ScriptIdentifier),
		offsetof(ScriptInstanceData, _CurrentScriptIdentifier)
	);
}

/*
*	Post-initializes this component.
*/
void ScriptComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void ScriptComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool ScriptComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void ScriptComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void ScriptComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	ScriptInitializationData *const RESTRICT _initialization_data{ static_cast<ScriptInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	ScriptInstanceData &instance_data{ _InstanceData.Back() };

	//Copy data.
	instance_data._PreviousScriptIdentifier = _initialization_data->_ScriptIdentifier;
	instance_data._CurrentScriptIdentifier = _initialization_data->_ScriptIdentifier;

	//Allocate the required data.
	const uint64 required_data_size{ GetRequiredDataSize(instance_data._CurrentScriptIdentifier) };

	if (required_data_size > 0)
	{
		instance_data._Data = Memory::Allocate(required_data_size);
	}
	
	else
	{
		instance_data._Data = nullptr;
	}

	//Set up the script context.
	ScriptContext script_context;

	script_context._Entity = entity;
	script_context._Data = instance_data._Data;

	//Initialize the script.
	InitializeScript(instance_data._CurrentScriptIdentifier, script_context);
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void ScriptComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{

}

/*
*	Destroys an instance.
*/
void ScriptComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance index.
	const uint64 instance_index{ EntityToInstance(entity) };

	//Cache the instance data.
	ScriptInstanceData &instance_data{ _InstanceData[instance_index] };

	//Set up the script context.
	ScriptContext script_context;

	script_context._Entity = entity;
	script_context._Data = instance_data._Data;

	//Terminate the script.
	TerminateScript(instance_data._CurrentScriptIdentifier, script_context);

	//Free the data.
	Memory::Free(instance_data._Data);

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 ScriptComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void ScriptComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::GAMEPLAY;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 1;
}

/*
*	Runs before the given update phase.
*/
void ScriptComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void ScriptComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("ScriptComponent::Update");

	switch (update_phase)
	{
		case UpdatePhase::GAMEPLAY:
		{
			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				ScriptInstanceData &instance_data{ _InstanceData[instance_index] };

				

				//Check if script has changed.
				if (instance_data._PreviousScriptIdentifier != instance_data._CurrentScriptIdentifier)
				{
					//Terminate the previous script.
					{
						//Set up the script context.
						ScriptContext script_context;

						script_context._Entity = InstanceToEntity(instance_index);
						script_context._Data = instance_data._Data;

						//Terminate the script.
						TerminateScript(instance_data._PreviousScriptIdentifier, script_context);

						//Free the data, if needed.
						if (instance_data._Data)
						{
							Memory::Free(instance_data._Data);
						}
					}

					//Set up the new script.
					{
						//Allocate the required data.
						const uint64 required_data_size{ GetRequiredDataSize(instance_data._CurrentScriptIdentifier) };

						if (required_data_size > 0)
						{
							instance_data._Data = Memory::Allocate(required_data_size);
						}

						else
						{
							instance_data._Data = nullptr;
						}

						//Set up the script context.
						ScriptContext script_context;

						script_context._Entity = InstanceToEntity(instance_index);
						script_context._Data = instance_data._Data;

						//Initialize the script!
						InitializeScript(instance_data._CurrentScriptIdentifier, script_context);
					}

					//Update the previous script identifier.
					instance_data._PreviousScriptIdentifier = instance_data._CurrentScriptIdentifier;
				}

				//Set up the script context.
				ScriptContext script_context;

				script_context._Entity = InstanceToEntity(instance_index);
				script_context._Data = instance_data._Data;

				//Update!
				UpdateScript(instance_data._CurrentScriptIdentifier, script_context);
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

/*
*	Runs after the given update phase.
*/
void ScriptComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}