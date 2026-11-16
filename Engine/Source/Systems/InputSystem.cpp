//Header file.
#include <Systems/InputSystem.h>

//Platform.
#include <Platform/Core/WindowManager.h>

//Profiling.
#include <Profiling/Profiling.h>

//Rendering.
#include <Rendering/Native/RenderPasses/ImGuiRenderPass.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>
#if !defined(CATALYST_CONFIGURATION_FINAL)
#include <Systems/ImGuiSystem.h>
#endif
#include <Systems/RenderingSystem.h>

/*
*	Initializes the input system.
*/
void InputSystem::Initialize() NOEXCEPT
{
	//Cache the configuration.
	const CatalystProjectInputConfiguration &configuration{ CatalystEngineSystem::Instance->GetProjectConfiguration()->_InputConfiguration };

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

	//Register the callbacks.
	Platform::WindowManager::AddKeyboardButtonDownCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const KeyboardButton keyboard_button, void *const RESTRICT user_data)
		{
			(*static_cast<KeyboardState *const RESTRICT>(user_data))[keyboard_button] = ButtonState::PRESSED;
			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::KEYBOARD;
		},
		&_InputState._KeyboardState
	);
	Platform::WindowManager::AddKeyboardButtonUpCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const KeyboardButton keyboard_button, void *const RESTRICT user_data)
		{
			(*static_cast<KeyboardState *const RESTRICT>(user_data))[keyboard_button] = ButtonState::RELEASED;
			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::KEYBOARD;
		},
		&_InputState._KeyboardState
	);
	Platform::WindowManager::AddMouseMoveCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const Vector2<float32> position, void *const RESTRICT user_data)
		{
			MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };
			
			mouse_state->_CurrentX = position._X;
			mouse_state->_CurrentY = position._Y;

			mouse_state->_DeltaX = mouse_state->_CurrentX - mouse_state->_PreviousX;
			mouse_state->_DeltaY = mouse_state->_CurrentY - mouse_state->_PreviousY;

			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
		},
		&_InputState._MouseState
	);
	Platform::WindowManager::AddMouseButtonDownCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const MouseButton mouse_button, void *const RESTRICT user_data)
		{
			MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };

			switch (mouse_button)
			{
				case MouseButton::LEFT:
				{
					mouse_state->_Left = ButtonState::PRESSED;

					break;
				}

				case MouseButton::MIDDLE:
				{
					mouse_state->_ScrollWheel = ButtonState::PRESSED;

					break;
				}

				case MouseButton::RIGHT:
				{
					mouse_state->_Right = ButtonState::PRESSED;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
		},
		&_InputState._MouseState
	);
	Platform::WindowManager::AddMouseButtonUpCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const MouseButton mouse_button, void *const RESTRICT user_data)
		{
			MouseState *const RESTRICT mouse_state{ static_cast<MouseState *const RESTRICT>(user_data) };

			switch (mouse_button)
			{
				case MouseButton::LEFT:
				{
					mouse_state->_Left = ButtonState::RELEASED;

					break;
				}

				case MouseButton::MIDDLE:
				{
					mouse_state->_ScrollWheel = ButtonState::RELEASED;

					break;
				}

				case MouseButton::RIGHT:
				{
					mouse_state->_Right = ButtonState::RELEASED;

					break;
				}

				default:
				{
					ASSERT(false, "Invalid case!");

					break;
				}
			}

			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
		},
		&_InputState._MouseState
	);
	Platform::WindowManager::AddMouseScrollWheelCallback
	(
		Platform::WindowManager::GetMasterWindow(),
		[](const int8 step, void *const RESTRICT user_data)
		{
			static_cast<MouseState *const RESTRICT>(user_data)->_ScrollWheelStep += step;
			InputSystem::Instance->_LastUpdatedInputDeviceType = InputDeviceType::MOUSE;
		},
		&_InputState._MouseState
	);
}

/*
*	Updates the input system.
*/
void InputSystem::Update(const UpdatePhase phase) NOEXCEPT
{
	switch (phase)
	{
		case UpdatePhase::INPUT:
		{
			UpdateInput();

			break;
		}

		case UpdatePhase::POST:
		{
			UpdatePost();

			break;
		}

		default:
		{
			ASSERT(false, "Invalid case!");

			break;
		}
	}
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
*	Updates during the input update phase.
*/
void InputSystem::UpdateInput() NOEXCEPT
{
	PROFILING_SCOPE("InputSystem::UpdateInput()");

	//Remember the old input state.
	const InputState old_input_state{ _InputState };

	//Update the gamepad states.
	{
		PROFILING_SCOPE("InputSystem::Update - UpdateGamepadStates");

		for (uint8 i{ 0 }; i < BaseMath::Minimum<uint8>(_NumberOfSupportedGamepads, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS); ++i)
		{
			UpdateGamepadState(i);
		}
	}

	//Determine the last updated input device type.
	{
		PROFILING_SCOPE("InputSystem::Update - DetermineLastUpdatedInputDeviceType");

		//Did the gamepad state change?
		if (!Memory::Compare(&old_input_state._GamepadStates, &_InputState._GamepadStates, sizeof(_InputState._GamepadStates)))
		{
			_LastUpdatedInputDeviceType = InputDeviceType::GAMEPAD;
		}
	}

#if !defined(CATALYST_CONFIGURATION_FINAL)
	//Let the ImGui system know input is available.
	if (ImGuiRenderPass::Instance->IsInitialized())
	{
		ImGuiSystem::Instance->OnInputAvailable();
	}
#endif
}

/*
*	Updates during the post update phase.
*/
void InputSystem::UpdatePost() NOEXCEPT
{
	//Update the keyboard state.
	for (ButtonState &button_state : _InputState._KeyboardState._ButtonStates)
	{
		if (button_state == ButtonState::PRESSED)
		{
			button_state = ButtonState::PRESSED_HELD;
		}

		else if (button_state == ButtonState::RELEASED)
		{
			button_state = ButtonState::RELEASED_HELD;
		}
	}

	//Update the mouse state.
	_InputState._MouseState._PreviousX = _InputState._MouseState._CurrentX;
	_InputState._MouseState._PreviousY = _InputState._MouseState._CurrentY;
	_InputState._MouseState._DeltaX = 0.0f;
	_InputState._MouseState._DeltaY = 0.0f;

	if (_InputState._MouseState._Left == ButtonState::PRESSED)
	{
		_InputState._MouseState._Left = ButtonState::PRESSED_HELD;
	}

	else if (_InputState._MouseState._Left == ButtonState::RELEASED)
	{
		_InputState._MouseState._Left = ButtonState::RELEASED_HELD;
	}

	if (_InputState._MouseState._ScrollWheel == ButtonState::PRESSED)
	{
		_InputState._MouseState._ScrollWheel = ButtonState::PRESSED_HELD;
	}

	else if (_InputState._MouseState._ScrollWheel == ButtonState::RELEASED)
	{
		_InputState._MouseState._ScrollWheel = ButtonState::RELEASED_HELD;
	}

	if (_InputState._MouseState._Right == ButtonState::PRESSED)
	{
		_InputState._MouseState._Right = ButtonState::PRESSED_HELD;
	}

	else if (_InputState._MouseState._Right == ButtonState::RELEASED)
	{
		_InputState._MouseState._Right = ButtonState::RELEASED_HELD;
	}

	_InputState._MouseState._ScrollWheelStep = 0;
}