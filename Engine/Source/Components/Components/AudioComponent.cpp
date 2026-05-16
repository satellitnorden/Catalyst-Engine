//Header file.
#include <Components/Components/AudioComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

/*
*	Initializes this component.
*/
void AudioComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableAudioAssetField
	(
		"Asset",
		offsetof(AudioInitializationData, _Asset),
		offsetof(AudioInstanceData, _Asset)
	);
}

/*
*	Creates an instance.
*/
void AudioComponent::CreateInstance(Entity *const RESTRICT entity, AudioInitializationData *const RESTRICT initialization_data, AudioInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Asset = initialization_data->_Asset;
}

/*
*	Destroys an instance.
*/
void AudioComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}