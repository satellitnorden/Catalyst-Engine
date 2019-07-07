//Header file.
#include <Systems/InputSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Updates the input system during the pre update phase.
*/
void InputSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_MouseState);
}