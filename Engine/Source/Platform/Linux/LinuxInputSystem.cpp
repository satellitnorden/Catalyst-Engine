#if defined(CATALYST_PLATFORM_LINUX)
//Header file.
#include <Systems/InputSystem.h>

/*
*	Updates the gamepad state at the given index.
*/
void InputSystem::UpdateGamepadState(const uint8 index) NOEXCEPT
{
	
}

/*
*	Updates the keyboard state.
*/
void InputSystem::UpdateKeyboardState() NOEXCEPT
{
	
}

/*
*	Updates the touch state.
*/
void InputSystem::UpdateTouchState() NOEXCEPT
{
	//Linux doesn't have touch. I think. (:
}
#endif