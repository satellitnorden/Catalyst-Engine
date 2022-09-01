#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input constants.
namespace InputConstants
{
	constexpr float32 GAMEPAD_DEADZONE{ 0.1f }; //0.0025f step.
	constexpr uint8 MAXIMUM_NUMBER_OF_GAMEPADS{ 4 };
	constexpr uint64 MAXIMUM_NUMBER_OF_TOUCH_FINGERS{ 4 };
}

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