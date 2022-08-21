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
    for (uint8 finger_index{ 0 }; finger_index < InputConstants::MAXIMUM_NUMBER_OF_TOUCH_FINGERS; ++finger_index)
    {
        if (CatalystPlatform::_CurrentAndroidTouchStates[finger_index]._IsCurrentlyTouched)
        {
            switch (_InputState._TouchState._FingerStates[finger_index]._ButtonState)
            {
                case ButtonState::PRESSED:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::PRESSED_HELD;

                    break;
                }

                case ButtonState::PRESSED_HELD:
                {
                    //Do nothing.

                    break;
                }

                case ButtonState::RELEASED:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::PRESSED;

                    break;
                }

                case ButtonState::RELEASED_HELD:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::PRESSED;

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
            switch (_InputState._TouchState._FingerStates[finger_index]._ButtonState)
            {
                case ButtonState::PRESSED:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::RELEASED;

                    break;
                }

                case ButtonState::PRESSED_HELD:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::RELEASED;

                    break;
                }

                case ButtonState::RELEASED:
                {
                    _InputState._TouchState._FingerStates[finger_index]._ButtonState = ButtonState::RELEASED_HELD;

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
        _InputState._TouchState._FingerStates[finger_index]._PreviousX = _InputState._TouchState._FingerStates[finger_index]._CurrentX;
        _InputState._TouchState._FingerStates[finger_index]._PreviousY = _InputState._TouchState._FingerStates[finger_index]._CurrentY;

        //Update the current X/Y coordinates.
        _InputState._TouchState._FingerStates[finger_index]._CurrentX = CatalystPlatform::_CurrentAndroidTouchStates[finger_index]._CurrentX;
        _InputState._TouchState._FingerStates[finger_index]._CurrentY = CatalystPlatform::_CurrentAndroidTouchStates[finger_index]._CurrentY;

        //Calculate the delta X/Y coordinates.
        _InputState._TouchState._FingerStates[finger_index]._DeltaX = _InputState._TouchState._FingerStates[finger_index]._CurrentX - _InputState._TouchState._FingerStates[finger_index]._PreviousX;
        _InputState._TouchState._FingerStates[finger_index]._DeltaY = _InputState._TouchState._FingerStates[finger_index]._CurrentY - _InputState._TouchState._FingerStates[finger_index]._PreviousY;
    }
}
#endif