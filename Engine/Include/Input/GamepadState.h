#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/InputCore.h>

class GamepadState final
{

public:

	bool _IsConnected{ false };

	ButtonState _DpadUp{ ButtonState::RELEASED_HELD };
	ButtonState _DpadDown{ ButtonState::RELEASED_HELD };
	ButtonState _DpadLeft{ ButtonState::RELEASED_HELD };
	ButtonState _DpadRight{ ButtonState::RELEASED_HELD };

	ButtonState _Start{ ButtonState::RELEASED_HELD };
	ButtonState _Back{ ButtonState::RELEASED_HELD };

	ButtonState _LeftThumb{ ButtonState::RELEASED_HELD };
	ButtonState _RightThumb{ ButtonState::RELEASED_HELD };

	ButtonState _LeftShoulder{ ButtonState::RELEASED_HELD };
	ButtonState _RightShoulder{ ButtonState::RELEASED_HELD };

	ButtonState _A{ ButtonState::RELEASED_HELD };
	ButtonState _B{ ButtonState::RELEASED_HELD };
	ButtonState _X{ ButtonState::RELEASED_HELD };
	ButtonState _Y{ ButtonState::RELEASED_HELD };

	float32 _PreviousLeftTrigger{ 0.0f };
	float32 _CurrentLeftTrigger{ 0.0f };
	float32 _PreviousRightTrigger{ 0.0f };
	float32 _CurrentRightTrigger{ 0.0f };

	float32 _LeftThumbstickX{ 0.0f };
	float32 _LeftThumbstickY{ 0.0f };

	float32 _RightThumbstickX{ 0.0f };
	float32 _RightThumbstickY{ 0.0f };

};