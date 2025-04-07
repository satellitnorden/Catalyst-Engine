//Header file.
#include <Components/Components/SoundComponent.h>

//Components.
#include <Components/Components/WorldTransformComponent.h>

//Systems.
#include <Systems/SoundSystem.h>

/*
*	Initializes this component.
*/
void SoundComponent::Initialize() NOEXCEPT
{
	//Add the editable fields.
	AddEditableSoundAssetField
	(
		"Sound",
		offsetof(SoundInitializationData, _Sound),
		offsetof(SoundInstanceData, _Sound)
	);
}

/*
*	Creates an instance.
*/
void SoundComponent::CreateInstance(Entity *const RESTRICT entity, SoundInitializationData *const RESTRICT initialization_data, SoundInstanceData *const RESTRICT instance_data) NOEXCEPT
{
	//Copy data.
	instance_data->_Sound = initialization_data->_Sound;
}

/*
*	Destroys an instance.
*/
void SoundComponent::DestroyInstance(Entity *const RESTRICT entity) NOEXCEPT
{
	//Remove the instance.
	RemoveInstance(entity);
}