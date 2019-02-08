//Header file.
#include <Entities/Types/SoundEntity.h>

//Components.
#include <Components/ComponentManager.h>

/*
*	Default constructor.
*/
SoundEntity::SoundEntity() NOEXCEPT
{
	//Set the entity type.
	_Type = EntityType::Sound;
}