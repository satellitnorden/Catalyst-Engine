#pragma once

//Core.
#include <Core/General/EngineCore.h>
#include <Core/Containers/StaticArray.h>

//Enumeration for all possible keyboard button states.
enum class KeyboardButtonState : uint8
{
	Pressed,
	PressedHold,
	Released,
	ReleasedHold
};

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

	Zero,
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,

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

	NumpadMultiply,
	NumpadAdd,
	NumpadSeparator,
	NumpadSubtract,
	NumpadDecimal,
	NumpadDivide,

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

	NumberOfKeyboardButtons
};

class KeyboardState final
{

public:

	/*
	*	Default constructor.
	*/
	KeyboardState() NOEXCEPT
	{
		//Set all keyboard button states to the default value.
		for (uint8 i = 0; i < INDEX(KeyboardButton::NumberOfKeyboardButtons); ++i)
		{
			keyboardButtonStates[i] = KeyboardButtonState::Released;
		}
	}

	/*
	*	Default destructor.
	*/
	~KeyboardState() NOEXCEPT
	{

	}

	/*
	*	Subscript operator overload, const.
	*/
	const KeyboardButtonState& operator[](const KeyboardButton button) const NOEXCEPT
	{
		return keyboardButtonStates[INDEX(button)];
	}

	/*
	*	Subscript operator overload, non-const.
	*/
	KeyboardButtonState& operator[](const KeyboardButton button) NOEXCEPT
	{
		return keyboardButtonStates[INDEX(button)];
	}

	/*
	*	Returns the keyboard button state for the given keyboard button.
	*/
	KeyboardButtonState GetKeyboardButtonState(const KeyboardButton button) const NOEXCEPT
	{
		return keyboardButtonStates[INDEX(button)];
	}

private:

	//The underlying keyboard button states.
	StaticArray<KeyboardButtonState, INDEX(KeyboardButton::NumberOfKeyboardButtons)> keyboardButtonStates{ };

};