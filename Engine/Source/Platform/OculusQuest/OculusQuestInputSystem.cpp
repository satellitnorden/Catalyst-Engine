#if defined(CATALYST_PLATFORM_OCULUS_QUEST)
//Header file.
#include <Systems/InputSystem.h>

/*
*	Updates the gamepad state at the given index.
*/
void InputSystem::UpdateGamepadState(const uint8 index) NOEXCEPT
{
	//Oculus Quest doesn't have gamepads. (:
}

/*
*	Updates the keyboard state.
*/
void InputSystem::UpdateKeyboardState() NOEXCEPT
{
	//Oculus Quest doesn't have keyboards. (:
}

/*
*	Updates the mouse state.
*/
void InputSystem::UpdateMouseState() NOEXCEPT
{
	//Oculus Quest doesn't have mouses. (:
}

/*
*	Updates the touch state.
*/
void InputSystem::UpdateTouchState() NOEXCEPT
{
	//Oculus Quest doesn't have touch. (:
}
#endif