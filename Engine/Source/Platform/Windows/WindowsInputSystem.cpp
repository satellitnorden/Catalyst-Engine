#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/InputSystem.h>

//Core.
#include <Core/General/Pair.h>

//Platform.
#include <Platform/Core/WindowManager.h>
#include <Platform/Platforms/Windows/CatalystPlatformWindows.h>

//Windows.
#include <Xinput.h>

//Windows input system logic.
namespace WindowsInputSystemLogic
{

	/*
	*	Updates a single gamepad button.
	*/
	FORCE_INLINE void UpdateGamepadButton(const WORD wButtons, const uint16 xInputButton, ButtonState *const RESTRICT button_state) NOEXCEPT
	{
		//The gamepad button is not pressed.
		if (!(wButtons & xInputButton))
		{
			if (*button_state == ButtonState::PRESSED_HELD || *button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::RELEASED;
			}

			else if (*button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::RELEASED_HELD;
			}	
		}

		//The gamepad button is pressed.
		else
		{
			if (*button_state == ButtonState::RELEASED_HELD || *button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::PRESSED;
			}

			else if (*button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::PRESSED_HELD;
			}
		}
	}

	/*
	*	Updates a single Windows button.
	*/
	FORCE_INLINE void UpdateWindowsButton(const uint16 button, ButtonState *const RESTRICT button_state) NOEXCEPT
	{
		//The Windows button is not pressed.
		if (!static_cast<bool>(GetKeyState(button) & 0x8000))
		{
			if (*button_state == ButtonState::PRESSED_HELD || *button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::RELEASED;
			}

			else if (*button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::RELEASED_HELD;
			}
		}

		//The Windows button is pressed.
		else
		{
			if (*button_state == ButtonState::RELEASED_HELD || *button_state == ButtonState::RELEASED)
			{
				*button_state = ButtonState::PRESSED;
			}

			else if (*button_state == ButtonState::PRESSED)
			{
				*button_state = ButtonState::PRESSED_HELD;
			}
		}
	}

}


/*
*	Updates the gamepad state at the given index.
*/
void InputSystem::UpdateGamepadState(const uint8 index) NOEXCEPT
{
	//The COM library needs to be initialized for XInput, so initialize that if not already done.
	static thread_local bool com_initialized{ false };

	if (!com_initialized)
	{
		CoInitialize(nullptr);

		com_initialized = true;
	}

	//No need to update if the window isn't in focus.
	if (!Platform::WindowManager::IsFocused(Platform::WindowManager::GetMasterWindow()))
	{
		return;
	}

	//Cache the state.
	GamepadState *const RESTRICT state{ &_InputState._GamepadStates[index] };

	XINPUT_STATE xInputState;

	if (XInputGetState(index, &xInputState) != ERROR_SUCCESS)
	{
		state->_IsConnected = false;

		return;
	}

	else
	{
		state->_IsConnected = true;
	}

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, &state->_DpadUp);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, &state->_DpadDown);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, &state->_DpadLeft);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, &state->_DpadRight);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_START, &state->_Start);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, &state->_Back);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, &state->_LeftThumb);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, &state->_RightThumb);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, &state->_LeftShoulder);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, &state->_RightShoulder);

	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_A, &state->_A);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_B, &state->_B);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_X, &state->_X);
	WindowsInputSystemLogic::UpdateGamepadButton(xInputState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, &state->_Y);

	state->_PreviousLeftTrigger = state->_CurrentLeftTrigger;
	state->_CurrentLeftTrigger = static_cast<float>(xInputState.Gamepad.bLeftTrigger) / 255;
	state->_PreviousRightTrigger = state->_CurrentRightTrigger;
	state->_CurrentRightTrigger = static_cast<float>(xInputState.Gamepad.bRightTrigger) / 255;

	state->_LeftThumbstickX = static_cast<float>(xInputState.Gamepad.sThumbLX) / 32'768;
	state->_LeftThumbstickY = static_cast<float>(xInputState.Gamepad.sThumbLY) / 32'768;

	state->_RightThumbstickX = static_cast<float>(xInputState.Gamepad.sThumbRX) / 32'768;
	state->_RightThumbstickY = static_cast<float>(xInputState.Gamepad.sThumbRY) / 32'768;

	//Calculate if the thumbstick values are in the deadzone.If so, reset them.
	if (BaseMath::Absolute(state->_LeftThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickX = 0.0f;
	}

	else
	{
		state->_LeftThumbstickX = BaseMath::Scale(state->_LeftThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_LeftThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_LeftThumbstickY = 0.0f;
	}

	else
	{
		state->_LeftThumbstickY = BaseMath::Scale(state->_LeftThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_RightThumbstickX) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickX = 0.0f;
	}

	else
	{
		state->_RightThumbstickX = BaseMath::Scale(state->_RightThumbstickX, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}

	if (BaseMath::Absolute(state->_RightThumbstickY) <= InputConstants::GAMEPAD_DEADZONE)
	{
		state->_RightThumbstickY = 0.0f;
	}

	else
	{
		state->_RightThumbstickY = BaseMath::Scale(state->_RightThumbstickY, InputConstants::GAMEPAD_DEADZONE, 1.0f, 0.0f, 1.0f);
	}
}
#endif