//Header file.
#include <Systems/InputSystem.h>

//Multithreading.
#include <Multithreading/Task.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Pre-updates the input system synchronously.
*/
void InputSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Execute the update task.
	updateTask.function = [](void *const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->UpdateSystemAsynchronous();
	};
	updateTask.arguments = this;

	TaskSystem::Instance->ExecuteTask(&updateTask);
}

/*
*	Post-updates the input system synchronously.
*/
void InputSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the update task to finish.
	updateTask.WaitFor();
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Update gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &currentGamepadStates[i]);
	}

	//Update the keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&currentKeyboardState);
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() NOEXCEPT
{
	
}