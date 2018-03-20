//Header file.
#include <ClairvoyantSoundDirector.h>

//Systems.
#include <Systems/InputSystem.h>
#include <Systems/SoundSystem.h>

//Clairvoyant.
#include <ClairvoyantSettings.h>

/*
*	Initializes the Clairvoyant sound director.
*/
void ClairvoyantSoundDirector::Initialize() NOEXCEPT
{
	//Load the sound banks.
	SoundSystem::Instance->LoadBank(CLAIRVOYANT_RESOURCES_FOLDER "Master Bank.bank");
	SoundSystem::Instance->LoadBank(CLAIRVOYANT_RESOURCES_FOLDER "Master Bank.strings.bank");
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
		const EventDescription *const RESTRICT windEventDescription{ SoundSystem::Instance->GetEventDescription("Kick") };
		SoundSystem::Instance->SubmitSoundRequest(SoundRequest(windEventDescription));
	}
	*/
}

/*
*	Releases the Clairvoyant sound director.
*/
void ClairvoyantSoundDirector::Release() NOEXCEPT
{

}