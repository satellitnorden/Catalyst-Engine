//Header file.
#include <Systems/InputSystem.h>

//Profiling.
#include <Profiling/Profiling.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/ImGuiSystem.h>
#endif
#include <Systems/RenderingSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Initializes the input system.
*/
void InputSystem::Initialize(const CatalystProjectInputConfiguration &configuration) NOEXCEPT
{
	//Set the number of supported gamepads.
	_NumberOfSupportedGamepads = configuration._NumberOfSupportedGamepads;

	//Guess the last updated input device type based on the platform.
#if defined(CATALYST_PLATFORM_ANDROID)
	_LastUpdatedInputDeviceType = InputDeviceType::TOUCH;
#elif defined(CATALYST_PLATFORM_WINDOWS)
	_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
#else
	_LastUpdatedInputDeviceType = InputDeviceType::UNKNOWN;
#endif

	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->InputUpdate();
	},
	this,
	UpdatePhase::INPUT,
	UpdatePhase::GAMEPLAY,
	false,
	false);
}

/*
*	Hides the cursor.
*/
void InputSystem::HideCursor(const InputLayer input_layer) const NOEXCEPT
{
	if (!IsInputLayerBlocked(input_layer))
	{
		CatalystPlatform::HideCursor();
	}
}

/*
*	Shows the cursor.
*/
void InputSystem::ShowCursor(const InputLayer input_layer) const NOEXCEPT
{
	if (!IsInputLayerBlocked(input_layer))
	{
		CatalystPlatform::ShowCursor();
	}
}

/*
*	Updates the input system during the input update phase.
*/
void InputSystem::InputUpdate() NOEXCEPT
{
	PROFILING_SCOPE("InputSystem_InputUpdate");

	//Remember the old input state.
	const InputState old_input_state{ _InputState };

	//Update the gamepad states.
	{
		PROFILING_SCOPE("InputSystem_InputUpdate_UpdateGamepadStates");

		for (uint8 i{ 0 }; i < BaseMath::Minimum<uint8>(_NumberOfSupportedGamepads, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS); ++i)
		{
			UpdateGamepadState(i);
		}
	}

	//Update the keyboard state.
	{
		PROFILING_SCOPE("InputSystem_InputUpdate_UpdateKeyboardState");

		UpdateKeyboardState();
	}

	//Update the mouse state.
	{
		PROFILING_SCOPE("InputSystem_InputUpdate_UpdateMouseState");

		UpdateMouseState();
	}

	//Update the touch state.
	{
		PROFILING_SCOPE("InputSystem_InputUpdate_UpdateTouchState");

		UpdateTouchState();
	}

	//Determine the last updated input device type.
	{
		PROFILING_SCOPE("InputSystem_InputUpdate_DetermineLastUpdatedInputDeviceType");

		//Did the gamepad state change?
		if (!Memory::Compare(&old_input_state._GamepadStates, &_InputState._GamepadStates, sizeof(_InputState._GamepadStates)))
		{
			_LastUpdatedInputDeviceType = InputDeviceType::GAMEPAD;
		}

		//Did the keybard state change?
		else if (!Memory::Compare(&old_input_state._KeyboardState, &_InputState._KeyboardState, sizeof(KeyboardState)))
		{
			_LastUpdatedInputDeviceType = InputDeviceType::KEYBOARD;
		}

		//Did the mouse state change?
		else if (!Memory::Compare(&old_input_state._MouseState, &_InputState._MouseState, sizeof(_InputState._MouseState)))
		{
			_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
		}

		//Did the touch state change?
		else if (!Memory::Compare(&old_input_state._TouchState, &_InputState._TouchState, sizeof(_InputState._TouchState)))
		{
			_LastUpdatedInputDeviceType = InputDeviceType::TOUCH;
		}
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Let the ImGui system know input is available.
	if (RenderingSystem::Instance->GetCurrentRenderingPath() != RenderingPath::CUSTOM)
	{
		ImGuiSystem::Instance->OnInputAvailable();
	}
#endif
}