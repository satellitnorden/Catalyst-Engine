//Header file.
#include <Entities/Types/SoundEntity.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Entities.
#include <Entities/Creation/SoundInitializationData.h>

//Systems.
#include <Systems/EntitySystem.h>
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
	const SoundInitializationData *const RESTRICT sound_initialization_data{ static_cast<const SoundInitializationData *const RESTRICT>(data) };

	component._Position = sound_initialization_data->_Position;

	//Destroy the initialization data.
	EntitySystem::Instance->DestroyInitializationData<SoundInitializationData>(data);
}

/*
*	Terminates this entity.
*/
void SoundEntity::Terminate() NOEXCEPT
{
	//Return this entitiy's components index.
	ComponentManager::ReturnSoundComponentsIndex(_ComponentsIndex);
}