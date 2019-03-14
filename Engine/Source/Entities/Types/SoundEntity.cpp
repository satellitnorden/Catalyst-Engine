//Header file.
#include <Entities/Types/SoundEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/SoundInitializationData.h>

//Systems.
#include <Systems/EntityCreationSystem.h>
#include <Systems/SoundSystem.h>

/*
*	Default constructor.
*/
SoundEntity::SoundEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Sound;
}

/*
*	Initializes this entity.
*/
void SoundEntity::Initialize(EntityInitializationData *const RESTRICT data) NOEXCEPT
{
	//Retrieve a new components index for this sound entity.
	_ComponentsIndex = ComponentManager::GetNewSoundComponentsIndex(this);

	//Copy the data over to the component.
	SoundComponent &component{ ComponentManager::GetSoundSoundComponents()[_ComponentsIndex] };
	const SoundInitializationData *const RESTRICT soundInitializationData{ static_cast<const SoundInitializationData *const RESTRICT>(data) };

	component._Position = soundInitializationData->_Position;

	//Create the sound instance.
	SoundSystem::Instance->CreateSoundInstance(soundInitializationData->_Description, &component._Instance);

	//Set the position of the sound instance.
	SoundSystem::Instance->SetPosition(component._Instance, component._Position);

	//Play the instance.
	SoundSystem::Instance->Play(component._Instance);

	//Destroy the initialization data.
	EntityCreationSystem::Instance->DestroyInitializationData<SoundInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void SoundEntity::Terminate() NOEXCEPT
{
	//Destroy the sound instance.
	SoundSystem::Instance->DestroySoundInstance(ComponentManager::GetSoundSoundComponents()[_ComponentsIndex]._Instance);

	//Return this entitiy's components index.
	ComponentManager::ReturnSoundComponentsIndex(_ComponentsIndex);
}