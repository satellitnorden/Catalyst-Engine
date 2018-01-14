//Header file.
#include <InputSystem.h>

//Input.
#include <InputUtilities.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Default constructor.
*/
InputSystem::InputSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
InputSystem::~InputSystem() CATALYST_NOEXCEPT
{

}

/*
*	Initializes the input system.
*/
void InputSystem::InitializeSystem() CATALYST_NOEXCEPT
{

}

/*
*	Updates the input system synchronously.
*/
void InputSystem::UpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Update buttons.
	updateButtons.store(true);
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() CATALYST_NOEXCEPT
{
	//Check if buttons should be updated.
	bool updateButtonsCopy{ false };
	bool expectedUpdateButtons{ true };

	if (updateButtons.compare_exchange_strong(expectedUpdateButtons, false))
		updateButtonsCopy = true;

	//If buttons should not be updated right now, yield the thread.
	if (!updateButtonsCopy)
		std::this_thread::yield();

	//Update gamepad states.
	for (uint8 i = 0; i < INPUT_MAXIMUM_GAMEPADS; ++i)
	{
		GamepadState currentGamepadStateCopy{ currentGamepadState[i].GetSafe() };

		InputUtilities::GetCurrentGamepadState(i, updateButtonsCopy, currentGamepadStateCopy);

		currentGamepadState[i].Set(currentGamepadStateCopy);
	}

	//Update the keyboard.
	if (updateButtonsCopy)
	{
		KeyboardState currentKeyboardStateCopy{ currentKeyboardState.GetUnsafe() };

		InputUtilities::GetCurrentKeyboardState(currentKeyboardStateCopy);

		currentKeyboardState.Set(currentKeyboardStateCopy);
	}
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	
}