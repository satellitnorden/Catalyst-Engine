#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input constants.
namespace InputConstants
{
	constexpr float32 DOUBLE_CLICK_THRESHOLD{ 0.5f };
	constexpr float32 GAMEPAD_DEADZONE{ 0.1f }; //0.0025f step.
	constexpr uint8 MAXIMUM_NUMBER_OF_GAMEPADS{ 4 };
	constexpr uint64 MAXIMUM_NUMBER_OF_TOUCH_FINGERS{ 4 };
}

//Enumeration covering all input layers.
enum class InputLayer : uint8
{
	GAME = BIT(0),
	USER_INTERFACE = BIT(1),
	DEBUG = BIT(2)
};

ENUMERATION_BIT_OPERATIONS(InputLayer);

//Enumeration for all input device type.
enum class InputDeviceType : uint8
{
	GAMEPAD,
	KEYBOARD,
	MOUSE,
	TOUCH,

	UNKNOWN
};

//Enumeration for all possible button states.
enum class ButtonState : uint8
{
	PRESSED,
	PRESSED_HELD,
	RELEASED,
	RELEASED_HELD
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

//Enumeration covering all mouse buttons.
enum class MouseButton : uint8
{
	LEFT,
	MIDDLE,
	RIGHT,

	NUMBER_OF_MOUSE_BUTTONS
};