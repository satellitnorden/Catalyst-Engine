#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Input.
#include <Input/InputCore.h>

//Enumeration for all keyboard buttons.
enum class KeyboardButton : uint8
{
	Backspace,
	Tab,
	Clear,
	Enter,
	Pause,
	CapsLock,
	Escape,
	Spacebar,
	PageUp,
	PageDown,
	Home,
	LeftArrow,
	UpArrow,
	RightArrow,
	DownArrow,
	Select,
	Print,
	Execute,
	PrintScreen,
	Insert,
	Delete,
	Help,

	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,

	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,

	LeftWindows,
	RightWindows,
	Applications,
	Sleep,

	NumpadZero,
	NumpadOne,
	NumpadTwo,
	NumpadThree,
	NumpadFour,
	NumpadFive,
	NumpadSix,
	NumpadSeven,
	NumpadEight,
	NumpadNine,

	MULTIPLY,
	ADD,
	SEPARATOR,
	SUBTRACT,
	DECIMAL,
	DIVIDE,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,

	NumLock,
	ScrollLock,
	LeftShift,
	RightShift,
	LeftControl,
	RightControl,
	LeftAlt,
	RightAlt,

	BrowserBack,
	BrowserForward,
	BrowserRefresh,
	BrowserStop,
	BrowserSearch,
	BrowserFavorites,
	BrowserStartAndHome,

	VolumeMute,
	VolumeDown,
	VolumeUp,

	NextTrack,
	PreviousTrack,
	StopMedia,
	PlayPause,
	
	COMMA,
	PERIOD,

	NumberOfKeyboardButtons
};

class KeyboardState final
{

public:

	//The input characters.
	DynamicArray<char> _InputCharacters;

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
	ButtonState& operator[](const KeyboardButton button) NOEXCEPT
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

private:

	//The underlying keyboard button states.
	StaticArray<ButtonState, UNDERLYING(KeyboardButton::NumberOfKeyboardButtons)> _ButtonStates{ };

};