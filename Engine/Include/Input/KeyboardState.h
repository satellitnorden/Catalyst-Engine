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

		//Set all upper/lower case characters to the default value.
		for (uint8 i = 0; i < UNDERLYING(KeyboardButton::NumberOfKeyboardButtons); ++i)
		{
			_UpperCaseCharacters[i] = CHAR_MAX;
			_LowerCaseCharacters[i] = CHAR_MAX;
		}

		//Set the upper/lower case characters.
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::ZERO)] = '0';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::ZERO)] = '0';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadZero)] = '0';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadZero)] = '0';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::ONE)] = '1';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::ONE)] = '1';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadOne)] = '1';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadOne)] = '1';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::TWO)] = '2';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::TWO)] = '2';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadTwo)] = '2';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadTwo)] = '2';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::THREE)] = '3';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::THREE)] = '3';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadThree)] = '3';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadThree)] = '3';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::FOUR)] = '4';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::FOUR)] = '4';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadFour)] = '4';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadFour)] = '4';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::FIVE)] = '5';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::FIVE)] = '5';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadFive)] = '5';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadFive)] = '5';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::SIX)] = '6';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::SIX)] = '6';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadSix)] = '6';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadSix)] = '6';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::SEVEN)] = '7';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::SEVEN)] = '7';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadSeven)] = '7';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadSeven)] = '7';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::EIGHT)] = '8';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::EIGHT)] = '8';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadEight)] = '8';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadEight)] = '8';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NINE)] = '9';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NINE)] = '9';
		_UpperCaseCharacters[UNDERLYING(KeyboardButton::NumpadNine)] = '9';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::NumpadNine)] = '9';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::A)] = 'A';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::A)] = 'a';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::B)] = 'B';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::B)] = 'b';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::C)] = 'C';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::C)] = 'c';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::D)] = 'D';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::D)] = 'd';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::E)] = 'E';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::E)] = 'e';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::F)] = 'F';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::F)] = 'f';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::G)] = 'G';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::G)] = 'g';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::H)] = 'H';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::H)] = 'h';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::I)] = 'I';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::I)] = 'i';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::J)] = 'J';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::J)] = 'j';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::K)] = 'K';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::K)] = 'k';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::L)] = 'L';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::L)] = 'l';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::M)] = 'M';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::M)] = 'm';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::N)] = 'N';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::N)] = 'n';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::O)] = 'O';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::O)] = 'o';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::P)] = 'P';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::P)] = 'p';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::Q)] = 'Q';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::Q)] = 'q';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::R)] = 'R';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::R)] = 'r';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::S)] = 'S';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::S)] = 's';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::T)] = 'T';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::T)] = 't';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::U)] = 'U';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::U)] = 'u';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::V)] = 'V';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::V)] = 'v';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::W)] = 'W';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::W)] = 'w';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::X)] = 'X';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::X)] = 'x';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::Y)] = 'Y';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::Y)] = 'y';

		_UpperCaseCharacters[UNDERLYING(KeyboardButton::Z)] = 'Z';
		_LowerCaseCharacters[UNDERLYING(KeyboardButton::Z)] = 'z';
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
	ButtonState& GetButtonState(const KeyboardButton button) NOEXCEPT
	{
		return _ButtonStates[UNDERLYING(button)];
	}

	/*
	*	Returns the upper case character for the given keyboard button.
	*/
	char GetUpperCaseCharacter(const KeyboardButton button) const NOEXCEPT
	{
		return _UpperCaseCharacters[UNDERLYING(button)];
	}

	/*
	*	Returns the lower case character for the given keyboard button.
	*/
	char GetLowerCaseCharacter(const KeyboardButton button) const NOEXCEPT
	{
		return _LowerCaseCharacters[UNDERLYING(button)];
	}

private:

	//The underlying keyboard button states.
	StaticArray<ButtonState, UNDERLYING(KeyboardButton::NumberOfKeyboardButtons)> _ButtonStates{ };

	//The upper case characters, if one is associated with a given keyboard button, otherwise CHAR_MAX.
	StaticArray<char, UNDERLYING(KeyboardButton::NumberOfKeyboardButtons)> _UpperCaseCharacters{ };

	//The lower case characters, if one is associated with a given keyboard button, otherwise CHAR_MAX.
	StaticArray<char, UNDERLYING(KeyboardButton::NumberOfKeyboardButtons)> _LowerCaseCharacters{ };

};