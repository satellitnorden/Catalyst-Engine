//Header file.
#include <Systems/InputSystem.h>

//Input.
#include <Input/InputUtilities.h>

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
	inputUpdateTask.function = [](void *const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->UpdateSystemAsynchronous();
	};
	inputUpdateTask.arguments = this;

	TaskSystem::Instance->ExecuteTask(&inputUpdateTask);
}

/*
*	Post-updates the input system synchronously.
*/
void InputSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the update task to finish.
	inputUpdateTask.WaitFor();
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() NOEXCEPT
{
#if defined(CATALYST_WINDOWS)
	//Update gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		InputUtilities::GetCurrentGamepadState(i, currentGamepadStates[i]);
	}

	//Update the keyboard.
	InputUtilities::GetCurrentKeyboardState(currentKeyboardState);
#endif
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() NOEXCEPT
{
	
}