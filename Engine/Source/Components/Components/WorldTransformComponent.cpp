//Header file.
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

DEFINE_COMPONENT(WorldTransformComponent, WorldTransformInitializationData, WorldTransformInstanceData);

/*
*	Initializes this component.
*/
void WorldTransformComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableWorldTransformField
	(
		"WorldTransform",
		offsetof(WorldTransformInitializationData, _WorldTransform),
		offsetof(WorldTransformInstanceData, _CurrentWorldTransform)
	);
}

/*
*	Post-initializes this component.
*/
void WorldTransformComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void WorldTransformComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool WorldTransformComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void WorldTransformComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void WorldTransformComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	WorldTransformInitializationData *const RESTRICT _initialization_data{ static_cast<WorldTransformInitializationData*const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	WorldTransformInstanceData &instance_data{ _InstanceData.Back() };

	//Copy data.
	instance_data._PreviousWorldTransform = instance_data._CurrentWorldTransform = _initialization_data->_WorldTransform;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void WorldTransformComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{

}

/*
*	Destroys an instance.
*/
void WorldTransformComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 WorldTransformComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void WorldTransformComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = UpdatePhase::PRE;
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 512;
}

/*
*	Runs before the given update phase.
*/
void WorldTransformComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void WorldTransformComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("WorldTransformComponent::Update");

	switch (update_phase)
	{
		case UpdatePhase::PRE:
		{
			//Iterate over the instances.
			for (uint64 instance_index{ start_instance_index }; instance_index < end_instance_index; ++instance_index)
			{
				//Cache the instance data.
				WorldTransformInstanceData &instance_data{ _InstanceData[instance_index] };

				//Update the previous world transform.
				instance_data._PreviousWorldTransform = instance_data._CurrentWorldTransform;
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
void WorldTransformComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}