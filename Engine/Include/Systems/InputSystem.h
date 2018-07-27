#pragma once

//Core.
#include <Core/EngineCore.h>

//Multithreading.
#include <Multithreading/Semaphore.h>
#include <Multithreading/MultithreadedResource.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/InputConstants.h>
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
	const GamepadState& GetCurrentGamepadState(uint8 index = 0) const NOEXCEPT { return currentGamepadStates[index]; }

private:

	//The update semaphore.
	Semaphore inputUpdateSemaphore;

	//The current gamepad states.
	StaticArray<GamepadState, INPUT_MAXIMUM_GAMEPADS> currentGamepadStates;

	//The current keyboard state.
	KeyboardState currentKeyboardState;

};