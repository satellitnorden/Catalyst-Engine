//Header file.
#include <ClairvoyantSoundDirector.h>

//Entities.
#include <Entities/Sound2DEntity.h>

//Systems.
#include <Systems/EntitySystem.h>
#include <Systems/InputSystem.h>
#include <Systems/SoundSystem.h>

//Clairvoyant.
#include <ClairvoyantSettings.h>

//Singleton definition.
DEFINE_SINGLETON(ClairvoyantSoundDirector);

/*
*	Initializes the Clairvoyant sound director.
*/
void ClairvoyantSoundDirector::Initialize() NOEXCEPT
{
	
}

/*
*	Updates the Clairvoyant sound director.
*/
void ClairvoyantSoundDirector::Update() NOEXCEPT
{
	/*
	//If the B button is pressed, play some sound.
	GamepadState currentGamepadState{ InputSystem::Instance->GetCurrentGamepadState() };

	if (currentGamepadState.aButtonState == GamepadButtonState::Pressed)
	{
		static const FMOD::Studio::EventDescription *const RESTRICT windEventDescription{ SoundSystem::Instance->GetEventDescription("Splatt") };

		Sound2DEntity *const RESTRICT sound{ EntitySystem::Instance->CreateEntity<Sound2DEntity>() };
		sound->Initialize(windEventDescription);
	}
	*/
}

/*
*	Releases the Clairvoyant sound director.
*/
void ClairvoyantSoundDirector::Release() NOEXCEPT
{

}