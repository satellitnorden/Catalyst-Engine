//Header file.
#include <Components/Components/WorldTransformComponent.h>

/*
*	Initializes this component.
*/
void WorldTransformComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableWorldTransformField
	(
		"World Transform",
		offsetof(WorldTransformInitializationData, _WorldTransform),
		offsetof(WorldTransformInstanceData, _CurrentWorldTransform)
	);
}

/*
*	Updates this component.
*/
void WorldTransformComponent::ParallelBatchUpdate(const UpdatePhase update_phase, const uint64 start_instance_index, const uint64 end_instance_index) NOEXCEPT
{
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
*	Creates an instance.
*/
void WorldTransformComponent::CreateInstance(Entity *const RESTRICT entity, WorldTransformInitializationData *const RESTRICT initialization_data, WorldTransformInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_PreviousWorldTransform = instance_data->_CurrentWorldTransform = initialization_data->_WorldTransform;
}

/*
*	Destroys an instance.
*/
void WorldTransformComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}