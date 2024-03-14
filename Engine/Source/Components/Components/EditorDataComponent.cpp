#if defined(CATALYST_EDITOR)
//Header file.
#include <Components/Components/EditorDataComponent.h>

DEFINE_COMPONENT(EditorDataComponent, EditorDataInitializationData, EditorDataInstanceData);

/*
*	Initializes this component.
*/
void EditorDataComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableEulerAnglesField
	(
		"Rotation",
		offsetof(EditorDataInitializationData, _Rotation),
		offsetof(EditorDataInstanceData, _Rotation)
	);
}

/*
*	Post-initializes this component.
*/
void EditorDataComponent::PostInitialize() NOEXCEPT
{

}

/*
*	Terminates this component.
*/
void EditorDataComponent::Terminate() NOEXCEPT
{

}

NO_DISCARD bool EditorDataComponent::NeedsPreProcessing() const NOEXCEPT
{
	return false;
}

/*
*	Preprocessed initialization data an instance.
*/
void EditorDataComponent::PreProcess(ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{

}

/*
*	Creates an instance.
*/
void EditorDataComponent::CreateInstance(Entity *const RESTRICT entity, ComponentInitializationData *const RESTRICT initialization_data) NOEXCEPT
{
	//Set up the instance data.
	EditorDataInitializationData *const RESTRICT _initialization_data{ static_cast<EditorDataInitializationData *const RESTRICT>(initialization_data) };
	_InstanceData.Emplace();
	EditorDataInstanceData &instance_data{ _InstanceData.Back() };

	//Set data.
	Memory::Set(&instance_data, 0, sizeof(EditorDataInstanceData));
}

/*
*	Runs after all components have created their instance for the given entity.
*	Useful if there is some setup needed involving multiple components.
*/
void EditorDataComponent::PostCreateInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	
}

/*
*	Destroys an instance.
*/
void EditorDataComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}

/*
*	Returns the number of sub-instances for the given instance.
*/
NO_DISCARD uint64 EditorDataComponent::NumberOfSubInstances(const uint64 instance_index) const NOEXCEPT
{
	return 1;
}

void EditorDataComponent::GetUpdateConfiguration(ComponentUpdateConfiguration *const RESTRICT update_configuration) NOEXCEPT
{
	update_configuration->_UpdatePhaseMask = static_cast<UpdatePhase>(0);
	update_configuration->_Mode = ComponentUpdateConfiguration::Mode::BATCH;
	update_configuration->_BatchSize = 0;
}

/*
*	Runs before the given update phase.
*/
void EditorDataComponent::PreUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}

/*
*	Updates this component.
*/
void EditorDataComponent::Update
(
	const UpdatePhase update_phase,
	const uint64 start_instance_index,
	const uint64 end_instance_index,
	const uint64 sub_instance_index
) NOEXCEPT
{
	
}

/*
*	Runs after the given update phase.
*/
void EditorDataComponent::PostUpdate(const UpdatePhase update_phase) NOEXCEPT
{

}
#endif