#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Input.
#include <Input/InputCore.h>

class GamepadState final
{

public:

	bool isConnected{ false };

	ButtonState dpadUpButtonState{ ButtonState::Released };
	ButtonState dpadDownButtonState{ ButtonState::Released };
	ButtonState dpadLeftButtonState{ ButtonState::Released };
	ButtonState dpadRightButtonState{ ButtonState::Released };

	ButtonState startButtonState{ ButtonState::Released };
	ButtonState backButtonState{ ButtonState::Released };

	ButtonState leftThumbButtonState{ ButtonState::Released };
	ButtonState rightThumbButtonState{ ButtonState::Released };

	ButtonState leftShoulderButtonState{ ButtonState::Released };
	ButtonState rightShoulderButtonState{ ButtonState::Released };

	ButtonState aButtonState{ ButtonState::Released };
	ButtonState bButtonState{ ButtonState::Released };
	ButtonState xButtonState{ ButtonState::Released };
	ButtonState yButtonState{ ButtonState::Released };

	float leftTriggerValue{ 0.0f };
	float rightTriggerValue{ 0.0f };

	float leftThumbstickXValue{ 0.0f };
	float leftThumbstickYValue{ 0.0f };

	float rightThumbstickXValue{ 0.0f };
	float rightThumbstickYValue{ 0.0f };

};