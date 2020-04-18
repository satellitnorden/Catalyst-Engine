#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/InputCore.h>

class GamepadState final
{

public:

	bool _IsConnected{ false };

	ButtonState _DpadUp{ ButtonState::RELEASED };
	ButtonState _DpadDown{ ButtonState::RELEASED };
	ButtonState _DpadLeft{ ButtonState::RELEASED };
	ButtonState _DpadRight{ ButtonState::RELEASED };

	ButtonState _Start{ ButtonState::RELEASED };
	ButtonState _Back{ ButtonState::RELEASED };

	ButtonState _LeftThumb{ ButtonState::RELEASED };
	ButtonState _RightThumb{ ButtonState::RELEASED };

	ButtonState _LeftShoulder{ ButtonState::RELEASED };
	ButtonState _RightShoulder{ ButtonState::RELEASED };

	ButtonState _A{ ButtonState::RELEASED };
	ButtonState _B{ ButtonState::RELEASED };
	ButtonState _X{ ButtonState::RELEASED };
	ButtonState _Y{ ButtonState::RELEASED };

	float _LeftTrigger{ 0.0f };
	float _RightTrigger{ 0.0f };

	float _LeftThumbstickX{ 0.0f };
	float _LeftThumbstickY{ 0.0f };

	float _RightThumbstickX{ 0.0f };
	float _RightThumbstickY{ 0.0f };

};