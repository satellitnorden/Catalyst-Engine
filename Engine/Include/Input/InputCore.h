#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input constants.
namespace InputConstants
{
	constexpr float GAMEPAD_DEADZONE{ 0.25f };
	constexpr uint8 MAXIMUM_NUMBER_OF_GAMEPADS{ 4 };
}

//Enumeration for all possible button states.
enum class ButtonState : uint8
{
	Pressed,
	PressedHold,
	Released,
	ReleasedHold
};