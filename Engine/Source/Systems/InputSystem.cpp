//Header file.
#include <Systems/InputSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Initializes the input system.
*/
void InputSystem::Initialize(const CatalystProjectInputConfiguration &configuration) NOEXCEPT
{
	//Set the number of supported gamepads.
	_NumberOfSupportedGamepads = configuration._NumberOfSupportedGamepads;

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->InputUpdate();
	},
	this,
	UpdatePhase::INPUT,
	UpdatePhase::USER_INTERFACE);
}

/*
*	Hides the cursor.
*/
void InputSystem::HideCursor() const NOEXCEPT
{
	CatalystPlatform::HideCursor();
}

/*
*	Shows the cursor.
*/
void InputSystem::ShowCursor() const NOEXCEPT
{
	CatalystPlatform::ShowCursor();
}

/*
*	Updates the input system during the input update phase.
*/
void InputSystem::InputUpdate() NOEXCEPT
{
	//Remember the old input state.
	const InputState old_input_state{ _InputState };

	//Retrieve the current gamepad states.
	for (uint8 i{ 0 }; i < CatalystBaseMath::Minimum<uint8>(_NumberOfSupportedGamepads, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS); ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_InputState._GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_InputState._KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_InputState._MouseState);

	//Determine the last updated input device type.
	{
		//Did the gamepad state change?
		if (!Memory::Compare(&old_input_state._GamepadStates, &_InputState._GamepadStates, sizeof(_InputState._GamepadStates)))
		{
			_LastUpdateInputDeviceType = InputDeviceType::GAMEPAD;
		}

		//Did the keybard state change?
		else if (!Memory::Compare(&old_input_state._KeyboardState, &_InputState._KeyboardState, sizeof(_InputState._KeyboardState)))
		{
			_LastUpdateInputDeviceType = InputDeviceType::KEYBOARD;
		}

		//Did the mouse state change?
		else if (!Memory::Compare(&old_input_state._MouseState, &_InputState._MouseState, sizeof(_InputState._MouseState)))
		{
			_LastUpdateInputDeviceType = InputDeviceType::MOUSE;
		}
	}
}