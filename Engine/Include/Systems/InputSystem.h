#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>

class InputSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InputSystem);

	/*
	*	Default constructor.
	*/
	InputSystem() NOEXCEPT { }

	/*
	*	Pre-updates the input system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Post-updates the input system synchronously.
	*/
	void PostUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Updates the input system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Releases the input system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the current gamepad state.
	*/
	NO_DISCARD const GamepadState& GetCurrentGamepadState(const uint8 index = 0) const NOEXCEPT { return currentGamepadStates[index]; }

	/*
	*	Returns the current keyboard state.
	*/
	NO_DISCARD RESTRICTED const KeyboardState *const RESTRICT GetCurrentKeyboardState() const NOEXCEPT
	{
		return &currentKeyboardState;
	}

private:

	//The update task.
	Task updateTask;

	//The current gamepad states.
	StaticArray<GamepadState, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS> currentGamepadStates;

	//The current keyboard state.
	KeyboardState currentKeyboardState;

};