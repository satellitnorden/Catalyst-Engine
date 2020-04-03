#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Concurrency.
#include <Concurrency/Task.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>
#include <Input/TouchState.h>

//Math.
#include <Math/General/Vector.h>

class InputSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InputSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE InputSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the input system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Updates the input system during the pre update phase.
	*/
	void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Returns the given gamepad state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const GamepadState *const RESTRICT GetGamepadState(const uint8 index = 0) const NOEXCEPT
	{
		return &_InputState._GamepadStates[index];
	}

	/*
	*	Returns the keyboard state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const KeyboardState *const RESTRICT GetKeyboardState() const NOEXCEPT
	{
		return &_InputState._KeyboardState;
	}

	/*
	*	Returns the mouse state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const MouseState *const RESTRICT GetMouseState() const NOEXCEPT
	{
		return &_InputState._MouseState;
	}

	/*
	*	Hides the cursor.
	*/
	void HideCursor() const NOEXCEPT;

	/*
	*	Shows the cursor.
	*/
	void ShowCursor() const NOEXCEPT;

private:

	/*
	*	Input stat class definition.
	*/
	class InputState final
	{


	public:

		//The gamepad states.
		StaticArray<GamepadState, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS> _GamepadStates;

		//The keyboard state.
		KeyboardState _KeyboardState;

		//The mouse state.
		MouseState _MouseState;

	};

	//The input state.
	InputState _InputState;

	//The asynchronous input state.
	InputState _AsynchronousInputState;

	//The update task.
	Task _UpdateTask;

	/*
	*	Updates the input system asynchronously.
	*/
	void UpdateAsynchronous() NOEXCEPT;

};