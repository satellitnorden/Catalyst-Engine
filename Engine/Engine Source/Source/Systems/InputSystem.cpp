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
*	Default constructor.
*/
InputSystem::InputSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
InputSystem::~InputSystem() NOEXCEPT
{

}

/*
*	Pre-updates the input system synchronously.
*/
void InputSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Execute the update task.
	TaskSystem::Instance->ExecuteTask(Task([](void *const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->UpdateSystemAsynchronous();
	}, this, &inputUpdateSemaphore));
}

/*
*	Post-updates the input system synchronously.
*/
void InputSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the update task to finish.
	inputUpdateSemaphore.WaitFor();
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() NOEXCEPT
{
	//Update gamepad states.
	for (uint8 i = 0; i < INPUT_MAXIMUM_GAMEPADS; ++i)
	{
		InputUtilities::GetCurrentGamepadState(i, currentGamepadStates[i]);
	}

	//Update the keyboard.
	InputUtilities::GetCurrentKeyboardState(currentKeyboardState);
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() NOEXCEPT
{
	
}