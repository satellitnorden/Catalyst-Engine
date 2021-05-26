#if defined(CATALYST_PLATFORM_ANDROID)
//Header file.
#include <Systems/InputSystem.h>

/*
*	Updates the gamepad state at the given index.
*/
void InputSystem::UpdateGamepadState(const uint8 index) NOEXCEPT
{
	//Android doesn't have gamepads. (:
}

/*
*	Updates the keyboard state.
*/
void InputSystem::UpdateKeyboardState() NOEXCEPT
{
	//Android doesn't have keyboards. (:
}

/*
*	Updates the mouse state.
*/
void InputSystem::UpdateMouseState() NOEXCEPT
{
	//Android doesn't have mouses. (:
}
#endif