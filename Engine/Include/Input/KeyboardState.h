#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Input.
#include <Input/InputCore.h>

class KeyboardState final
{

public:

	//The underlying keyboard button states.
	StaticArray<ButtonState, UNDERLYING(KeyboardButton::NumberOfKeyboardButtons)> _ButtonStates{ };

	/*
	*	Default constructor.
	*/
	KeyboardState() NOEXCEPT
	{
		//Set all keyboard button states to the default value.
		for (uint8 i = 0; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
		{
			_ButtonStates[i] = ButtonState::RELEASED;
		}
	}

	/*
	*	Subscript operator overload, const.
	*/
	ButtonState operator[](const KeyboardButton button) const NOEXCEPT
	{
		return _ButtonStates[UNDERLYING(button)];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	ButtonState &operator[](const KeyboardButton button) NOEXCEPT
	{
		return _ButtonStates[UNDERLYING(button)];
	}

	/*
	*	Returns the keyboard button state for the given keyboard button, const.
	*/
	ButtonState GetButtonState(const KeyboardButton button) const NOEXCEPT
	{
		return _ButtonStates[UNDERLYING(button)];
	}

	/*
	*	Returns the keyboard button state for the given keyboard button, non-const.
	*/
	ButtonState &GetButtonState(const KeyboardButton button) NOEXCEPT
	{
		return _ButtonStates[UNDERLYING(button)];
	}

};