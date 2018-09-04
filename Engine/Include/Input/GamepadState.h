#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Input.
#include <Input/InputCore.h>

class GamepadState final
{

public:

	bool _IsConnected{ false };

	ButtonState _DpadUp{ ButtonState::Released };
	ButtonState _DpadDown{ ButtonState::Released };
	ButtonState _DpadLeft{ ButtonState::Released };
	ButtonState _DpadRight{ ButtonState::Released };

	ButtonState _Start{ ButtonState::Released };
	ButtonState _Back{ ButtonState::Released };

	ButtonState _LeftThumb{ ButtonState::Released };
	ButtonState _RightThumb{ ButtonState::Released };

	ButtonState _LeftShoulder{ ButtonState::Released };
	ButtonState _RightShoulder{ ButtonState::Released };

	ButtonState _A{ ButtonState::Released };
	ButtonState _B{ ButtonState::Released };
	ButtonState _X{ ButtonState::Released };
	ButtonState _Y{ ButtonState::Released };

	float _LeftTrigger{ 0.0f };
	float _RightTrigger{ 0.0f };

	float _LeftThumbstickX{ 0.0f };
	float _LeftThumbstickY{ 0.0f };

	float _RightThumbstickX{ 0.0f };
	float _RightThumbstickY{ 0.0f };

};