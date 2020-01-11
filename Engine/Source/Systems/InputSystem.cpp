//Header file.
#include <Systems/InputSystem.h>

//Systems.
#include <Systems/TaskSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Initializes the input system.
*/
void InputSystem::Initialize() NOEXCEPT
{
	//Set up the update task.
	_UpdateTask._Function = [](void *const RESTRICT)
	{
		InputSystem::Instance->UpdateAsynchronous();
	};
	_UpdateTask._Arguments = nullptr;
	_UpdateTask._ExecutableOnSameThread = false;
}

/*
*	Updates the input system during the pre update phase.
*/
void InputSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Wait for the update task to finish.
	_UpdateTask.WaitFor();

	//Copy over the asynchronous input state.
	Memory::Copy(&_InputState, &_AsynchronousInputState, sizeof(InputState));

	//Fire off the update task again.
	TaskSystem::Instance->ExecuteTask(&_UpdateTask);
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
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateAsynchronous() NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_AsynchronousInputState._GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_AsynchronousInputState._KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_AsynchronousInputState._MouseState);
}