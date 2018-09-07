//Header file.
#include <Systems/InputSystem.h>

//Multithreading.
#include <Multithreading/Task.h>

//Systems.
#include <Systems/TaskSystem.h>
#include <Systems/UpdateSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Initializes the input system.
*/
void InputSystem::InitializeSystem() NOEXCEPT
{
	//Initialize the update task.
	_UpdateTask._Function = [](void *const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->UpdateSystemAsynchronous();
	};
	_UpdateTask._Arguments = this;
}

/*
*	Pre-updates the input system synchronously.
*/
void InputSystem::PreUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Execute the update task.
	TaskSystem::Instance->ExecuteTask(&_UpdateTask);
}

/*
*	Post-updates the input system synchronously.
*/
void InputSystem::PostUpdateSystemSynchronous(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Wait for the update task to finish.
	_UpdateTask.WaitFor();
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_MouseState);

	//Retrieve the current touch state.
	CatalystPlatform::GetCurrentTouchState(&_TouchState);
}