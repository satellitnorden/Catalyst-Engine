#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/CatalystProjectConfiguration.h>

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
	void Initialize(const CatalystProjectInputConfiguration &configuration) NOEXCEPT;

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
	*	Returns the last update input device type.
	*/
	FORCE_INLINE NO_DISCARD InputDeviceType GetLastUpdatedInputDeviceType() const NOEXCEPT
	{
		return _LastUpdatedInputDeviceType;
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

	//The number of supported gamepads.
	uint8 _NumberOfSupportedGamepads;

	//The input state.
	InputState _InputState;

	//Denotes the last updated input device type.
	InputDeviceType _LastUpdatedInputDeviceType;

	/*
	*	Updates the input system during the input update phase.
	*/
	void InputUpdate() NOEXCEPT;

	/*
	*	Updates the gamepad state at the given index.
	*/
	void UpdateGamepadState(const uint8 index) NOEXCEPT;

	/*
	*	Updates the keyboard state.
	*/
	void UpdateKeyboardState() NOEXCEPT;

	/*
	*	Updates the mouse state.
	*/
	void UpdateMouseState() NOEXCEPT;

};