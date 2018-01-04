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
	//Update gamepad buttons.
	updateGamepadButtons.store(true);
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() CATALYST_NOEXCEPT
{
	bool updateGamepadButtonsCopy{ false };
	bool expectedUpdateHold{ true };

	if (updateGamepadButtons.compare_exchange_strong(expectedUpdateHold, false))
		updateGamepadButtonsCopy = true;

	for (uint8 i = 0; i < 4; ++i)
	{
		GamepadState currentGamepadStateCopy{ currentGamepadState[i].GetSafe() };

		InputUtilities::GetCurrentGamepadState(i, updateGamepadButtonsCopy, currentGamepadStateCopy);

		currentGamepadState[i].Set(currentGamepadStateCopy);
	}
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	
}