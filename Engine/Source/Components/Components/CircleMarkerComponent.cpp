//Header file.
#include <Components/Components/CircleMarkerComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#if defined(CATALYST_EDITOR)
#include <Systems/CatalystEditorSystem.h>
#endif
#include <Systems/RenderingSystem.h>
#if defined(CATALYST_EDITOR)
#include <Systems/WorldSystem.h>
#endif

DEFINE_COMPONENT(CircleMarkerComponent, CircleMarkerInitializationData, CircleMarkerInstanceData);

#if defined(CATALYST_EDITOR)
/*
*	Circle marker push constant data.
*/
class CircleMarkerPushConstantData final
{

public:

	//The position/radius.
	Vector4<float32> _PositionRadius;

};

/*
*	Gathers a circle marker render input stream.
*/
void GatherCircleMarkerRenderInputStream(RenderInputStream *const RESTRICT input_stream) NOEXCEPT
{
	//Clear the entries.
	input_stream->_Entries.Clear();

	//Clear the push constant data memory.
	input_stream->_PushConstantDataMemory.Clear();

	if (!CatalystEditorSystem::Instance->IsInGame())
	{
		//Go through all instances.
		for (uint64 instance_index{ 0 }; instance_index < CircleMarkerComponent::Instance->NumberOfInstances(); ++instance_index)
		{
			//Cache data.
			Entity *const RESTRICT entity{ CircleMarkerComponent::Instance->InstanceToEntity(instance_index) };
			const CircleMarkerInstanceData &circle_marker_instance_data{ CircleMarkerComponent::Instance->InstanceData(entity) };
			const WorldTransformInstanceData &world_transform_instance_data{ WorldTransformComponent::Instance->InstanceData(entity) };

			//Add a new entry.
			input_stream->_Entries.Emplace();
			RenderInputStreamEntry &new_entry{ input_stream->_Entries.Back() };

			new_entry._PushConstantDataOffset = input_stream->_PushConstantDataMemory.Size();
			new_entry._VertexBuffer = EMPTY_HANDLE;
			new_entry._IndexBuffer = EMPTY_HANDLE;
			new_entry._IndexBufferOffset = 0;
			new_entry._InstanceBuffer = EMPTY_HANDLE;
			new_entry._VertexCount = 66;
			new_entry._IndexCount = 0;
			new_entry._InstanceCount = 0;

			//Set up the push constant data.
			CircleMarkerPushConstantData push_constant_data;

			push_constant_data._PositionRadius = Vector4<float32>
			(
				world_transform_instance_data._CurrentWorldTransform.GetRelativePosition(WorldSystem::Instance->GetCurrentWorldGridCell()),
				circle_marker_instance_data._Radius
			);

			for (uint64 i{ 0 }; i < sizeof(CircleMarkerPushConstantData); ++i)
			{
				input_stream->_PushConstantDataMemory.Emplace(((const byte *const RESTRICT)&push_constant_data)[i]);
			}
		}
	}
}
#endif

/*
*	Initializes this component.
*/
void CircleMarkerComponent::Initialize() NOEXCEPT
{
#if defined(CATALYST_EDITOR)
	//Register the input stream.
	RenderingSystem::Instance->GetRenderInputManager()->RegisterInputStream
	(
		HashString("CircleMarker"),
		DynamicArray<VertexInputAttributeDescription>(),
		DynamicArray<VertexInputBindingDescription>(),
		sizeof(CircleMarkerPushConstantData),
		[](void *const RESTRICT user_data, RenderInputStream *const RESTRICT input_stream)
		{
			GatherCircleMarkerRenderInputStream(input_stream);
		},
		RenderInputStream::Mode::DRAW,
		nullptr
	);
#endif

	//Add the editable fields.
	AddEditableFloatField
	(
		"Radius",
		offsetof(CircleMarkerInitializationData, _Radius),
		offsetof(CircleMarkerInstanceData, _Radius)
	);
}

/*
*	Creates an instance.
*/
void CircleMarkerComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	CircleMarkerInitializationData *const RESTRICT _initialization_data{ static_cast<CircleMarkerInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	CircleMarkerInstanceData &instance_data{ _InstanceData.Back() };

	instance_data._Radius = _initialization_data->_Radius;
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void CircleMarkerComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	ASSERT(WorldTransformComponent::Instance->Has(entity), "Circle marker component needs a world transform component!");
}

/*
*	Destroys an instance.
*/
void CircleMarkerComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 CircleMarkerComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void CircleMarkerComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = static_cast<UpdatePhase>(0);
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 0;
}

/*
*	Runs before the given update phase.
*/
void CircleMarkerComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void CircleMarkerComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	PROFILING_SCOPE("CircleMarkerComponent::Update");

	ASSERT(false, "This does not need to be updated. (:");
}

/*
*	Runs after the given update phase.
*/
void CircleMarkerComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}