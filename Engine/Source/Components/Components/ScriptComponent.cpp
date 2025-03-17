//Header file.
#include <Components/Components/ScriptComponent.h>

//Generated
#include <Generated/Script.Generated.h>

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
		offsetof(ScriptInstanceData, _ScriptIdentifier)
	);
}

/*
*	Updates this component.
*/
void ScriptComponent::SerialUpdate(const UpdatePhase update_phase) NOEXCEPT
{
	switch (update_phase)
	{
		case UpdatePhase::GAMEPLAY:
		{
			//Iterate over the instances.
			for (uint64 instance_index{ 0 }; instance_index < NumberOfInstances(); ++instance_index)
			{
				//Cache the instance data.
				ScriptInstanceData &instance_data{ _InstanceData[instance_index] };

				//Set up the script context.
				ScriptContext script_context;

				script_context._Entity = InstanceToEntity(instance_index);
				script_context._Data = instance_data._Data;

				//Update!
				Script::Update(instance_data._ScriptIdentifier, script_context);
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
*	Creates an instance.
*/
void ScriptComponent::CreateInstance(Entity *const RESTRICT entity, ScriptInitializationData *const RESTRICT initialization_data, ScriptInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_ScriptIdentifier = initialization_data->_ScriptIdentifier;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void ScriptComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Cache the instance data.
	ScriptInstanceData &instance_data{ InstanceData(entity) };

	//Allocate the required data.
	const uint64 required_data_size{ Script::RequiredDataSize(instance_data._ScriptIdentifier) };

	if (required_data_size > 0)
	{
		instance_data._Data = Memory::Allocate(required_data_size);
		Memory::Set(instance_data._Data, 0, required_data_size);
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
	Script::Initialize(instance_data._ScriptIdentifier, script_context);
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
	Script::Terminate(instance_data._ScriptIdentifier, script_context);

	//Free the data.
	Memory::Free(instance_data._Data);

	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Callback for before an editable field change happens.
*/
void ScriptComponent::PreEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{
	//Cache the instance data.
	ScriptInstanceData &instance_data{ InstanceData(entity) };

	if (editable_field._Identifier == HashString("Script"))
	{
		//Set up the script context.
		ScriptContext script_context;

		script_context._Entity = entity;
		script_context._Data = instance_data._Data;

		//Terminate the current script.
		Script::Terminate(instance_data._ScriptIdentifier, script_context);

		//Free the data.
		Memory::Free(instance_data._Data);
	}
}

/*
*	Callback for after an editable field change happens.
*/
void ScriptComponent::PostEditableFieldChange(Entity *const RESTRICT entity, const ComponentEditableField &editable_field) NOEXCEPT
{
	//Cache the instance data.
	ScriptInstanceData &instance_data{ InstanceData(entity) };

	if (editable_field._Identifier == HashString("Script"))
	{
		//Allocate the required data.
		const uint64 required_data_size{ Script::RequiredDataSize(instance_data._ScriptIdentifier) };

		if (required_data_size > 0)
		{
			instance_data._Data = Memory::Allocate(required_data_size);
			Memory::Set(instance_data._Data, 0, required_data_size);
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
		Script::Initialize(instance_data._ScriptIdentifier, script_context);
	}
}

/*
*	Sends the given event to the given script.
*/
void ScriptComponent::Event(Entity *const RESTRICT entity, const HashString event) NOEXCEPT
{	
	//Cache the instance data.
	ScriptInstanceData &instance_data{ InstanceData(entity) };

	//Set up the script context.
	ScriptContext script_context;

	script_context._Entity = entity;
	script_context._Data = instance_data._Data;

	//Send the event!
	Script::Event(instance_data._ScriptIdentifier, event, script_context);
}