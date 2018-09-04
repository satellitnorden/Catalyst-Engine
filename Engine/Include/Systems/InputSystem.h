#pragma once

//Core.
#include <Core/Core/CatalystCore.h>
#include <Core/Pointers/UniquePointer.h>

//Multithreading.
#include <Multithreading/Task.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>
#include <Input/TouchState.h>

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
	*	Initializes the input system.
	*/
	void InitializeSystem() NOEXCEPT;

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
	*	Returns the current gamepad state.
	*/
	NO_DISCARD RESTRICTED const GamepadState *const RESTRICT GetGamepadState(const uint8 index = 0) const NOEXCEPT
	{
		return &_GamepadStates[index];
	}

	/*
	*	Returns the current keyboard state.
	*/
	NO_DISCARD RESTRICTED const KeyboardState *const RESTRICT GetKeyboardState() const NOEXCEPT
	{
		return &_KeyboardState;
	}

	/*
	*	Returns the current mouse state.
	*/
	NO_DISCARD RESTRICTED const MouseState *const RESTRICT GetMouseState() const NOEXCEPT
	{
		return &_MouseState;
	}

	/*
	*	Returns the current touch state.
	*/
	NO_DISCARD RESTRICTED const TouchState *const RESTRICT GetTouchState() const NOEXCEPT
	{
		return &_TouchState;
	}

private:

	//The update task.
	Task _UpdateTask;

	//The gamepad states.
	StaticArray<GamepadState, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS> _GamepadStates;

	//The keyboard state.
	KeyboardState _KeyboardState;

	//The mouse state.
	MouseState _MouseState;

	//The touch state.
	TouchState _TouchState;

};