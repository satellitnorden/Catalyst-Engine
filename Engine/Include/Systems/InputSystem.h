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

//Systems.
#include <Systems/System.h>

class InputSystem final
{

public:

	//System declaration.
	CATALYST_SYSTEM
	(
		InputSystem,
		SYSTEM_INITIALIZE()
	);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE InputSystem() NOEXCEPT
	{

	}

	/*
	*	Locks an input layer.
	*/
	FORCE_INLINE void LockInputLayer(const InputLayer input_layer) NOEXCEPT
	{
		SET_BIT(_LockedInputLayers, input_layer);
	}

	/*
	*	Unlocks an input layer.
	*/
	FORCE_INLINE void UnlockInputLayer(const InputLayer input_layer) NOEXCEPT
	{
		CLEAR_BIT(_LockedInputLayers, input_layer);
	}

	/*
	*	Returns if an input layer is locked.
	*/
	FORCE_INLINE NO_DISCARD bool IsInputLayerBlocked(const InputLayer input_layer) const NOEXCEPT
	{
		return TEST_BIT(_LockedInputLayers, input_layer);
	}

	/*
	*	Returns the given gamepad state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const GamepadState *const RESTRICT GetGamepadState(const InputLayer input_layer, const uint8 index = 0) const NOEXCEPT
	{
		return IsInputLayerBlocked(input_layer) ? &_EmptyInputState._GamepadStates[index] : &_InputState._GamepadStates[index];
	}

	/*
	*	Returns the keyboard state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const KeyboardState *const RESTRICT GetKeyboardState(const InputLayer input_layer) const NOEXCEPT
	{
		return IsInputLayerBlocked(input_layer) ? &_EmptyInputState._KeyboardState : &_InputState._KeyboardState;
	}

	/*
	*	Returns the mouse state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const MouseState *const RESTRICT GetMouseState(const InputLayer input_layer) const NOEXCEPT
	{
		return IsInputLayerBlocked(input_layer) ? &_EmptyInputState._MouseState : &_InputState._MouseState;
	}

	/*
	*	Returns the touch state.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const TouchState *const RESTRICT GetTouchState(const InputLayer input_layer) const NOEXCEPT
	{
		return IsInputLayerBlocked(input_layer) ? &_EmptyInputState._TouchState : &_InputState._TouchState;
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
	void HideCursor(const InputLayer input_layer) const NOEXCEPT;

	/*
	*	Shows the cursor.
	*/
	void ShowCursor(const InputLayer input_layer) const NOEXCEPT;

	/*
	*	Sets the number of supported gamepads.
	*/
	FORCE_INLINE void SetNumberOfSupportedGamepads(const uint8 value) NOEXCEPT
	{
		_NumberOfSupportedGamepads = value;
	}

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

		//The touch state.
		TouchState _TouchState;

	};

	//The locked input layers.
	InputLayer _LockedInputLayers{ static_cast<InputLayer>(0) };

	//The number of supported gamepads.
	uint8 _NumberOfSupportedGamepads;

	//The empty input state.
	InputState _EmptyInputState;

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

	/*
	*	Updates the touch state.
	*/
	void UpdateTouchState() NOEXCEPT;

};