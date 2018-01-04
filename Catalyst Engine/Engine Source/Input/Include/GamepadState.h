#pragma once

//Engine core.
#include <EngineCore.h>

//Defines.
#define GAMEPAD_DEADZONE 0.25f

//Enumeration for all possible gamepad button states.
enum class GamepadButtonState : uint8
{
	Pressed,
	PressedHold,
	Released,
	ReleasedHold
};

class GamepadState
{

public:

	bool isConnected{ false };

	GamepadButtonState dpadUpButtonState{ GamepadButtonState::Released };
	GamepadButtonState dpadDownButtonState{ GamepadButtonState::Released };
	GamepadButtonState dpadLeftButtonState{ GamepadButtonState::Released };
	GamepadButtonState dpadRightButtonState{ GamepadButtonState::Released };

	GamepadButtonState startButtonState{ GamepadButtonState::Released };
	GamepadButtonState backButtonState{ GamepadButtonState::Released };

	GamepadButtonState leftThumbButtonState{ GamepadButtonState::Released };
	GamepadButtonState rightThumbButtonState{ GamepadButtonState::Released };

	GamepadButtonState leftShoulderButtonState{ GamepadButtonState::Released };
	GamepadButtonState rightShoulderButtonState{ GamepadButtonState::Released };

	GamepadButtonState aButtonState{ GamepadButtonState::Released };
	GamepadButtonState bButtonState{ GamepadButtonState::Released };
	GamepadButtonState xButtonState{ GamepadButtonState::Released };
	GamepadButtonState yButtonState{ GamepadButtonState::Released };

	float leftTriggerValue{ 0.0f };
	float rightTriggerValue{ 0.0f };

	float leftThumbstickXValue{ 0.0f };
	float leftThumbstickYValue{ 0.0f };

	float rightThumbstickXValue{ 0.0f };
	float rightThumbstickYValue{ 0.0f };

	/*
	*	Default constructor.
	*/
	GamepadState() CATALYST_NOEXCEPT
	{

	}

};