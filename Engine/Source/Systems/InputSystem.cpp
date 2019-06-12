//Header file.
#include <Systems/InputSystem.h>

//Components.
#include <Components/Core/ComponentManager.h>

//Multithreading.
#include <Multithreading/Task.h>

//Systems.
#include <Systems/TaskSystem.h>

/*
	*	Updates the input system during the pre update phase.
	*/
void InputSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &ComponentManager::WriteSingletonComponent<InputComponent>()->_GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&ComponentManager::WriteSingletonComponent<InputComponent>()->_KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&ComponentManager::WriteSingletonComponent<InputComponent>()->_MouseState);

	//Retrieve the current touch state.
	CatalystPlatform::GetCurrentTouchState(&ComponentManager::WriteSingletonComponent<InputComponent>()->_TouchState);
}