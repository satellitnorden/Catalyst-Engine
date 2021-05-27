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

/*
*	Updates the touch state.
*/
void InputSystem::UpdateTouchState() NOEXCEPT
{
    //Update the button state.
    if (CatalystPlatform::_CurrentAndroidTouchState._IsCurrentlyTouched)
    {
        switch (_InputState._TouchState._ButtonState)
        {
            case ButtonState::PRESSED:
            {
                _InputState._TouchState._ButtonState = ButtonState::PRESSED_HELD;

                break;
            }

            case ButtonState::PRESSED_HELD:
            {
                //Do nothing.

                break;
            }

            case ButtonState::RELEASED:
            {
                _InputState._TouchState._ButtonState = ButtonState::PRESSED;

                break;
            }

            case ButtonState::RELEASED_HELD:
            {
                _InputState._TouchState._ButtonState = ButtonState::PRESSED;

                break;
            }

            default:
            {
                ASSERT(false, "Invalid case!");

                break;
            }
        }
    }

    else
    {
        switch (_InputState._TouchState._ButtonState)
        {
            case ButtonState::PRESSED:
            {
                _InputState._TouchState._ButtonState = ButtonState::RELEASED;

                break;
            }

            case ButtonState::PRESSED_HELD:
            {
                _InputState._TouchState._ButtonState = ButtonState::RELEASED;

                break;
            }

            case ButtonState::RELEASED:
            {
                _InputState._TouchState._ButtonState = ButtonState::RELEASED_HELD;

                break;
            }

            case ButtonState::RELEASED_HELD:
            {
                //Do nothing.

                break;
            }

            default:
            {
                ASSERT(false, "Invalid case!");

                break;
            }
        }
    }

	//Copy the previous X/Y coordinates.
    _InputState._TouchState._PreviousX = _InputState._TouchState._CurrentX;
    _InputState._TouchState._PreviousY = _InputState._TouchState._CurrentY;

    //Update the current X/Y coordinates.
    _InputState._TouchState._CurrentX = CatalystPlatform::_CurrentAndroidTouchState._CurrentX;
    _InputState._TouchState._CurrentY = CatalystPlatform::_CurrentAndroidTouchState._CurrentY;

    //Calculate the delta X/Y coordinates.
    _InputState._TouchState._DeltaX = _InputState._TouchState._CurrentX - _InputState._TouchState._PreviousX;
    _InputState._TouchState._DeltaY = _InputState._TouchState._CurrentY - _InputState._TouchState._PreviousY;
}
#endif