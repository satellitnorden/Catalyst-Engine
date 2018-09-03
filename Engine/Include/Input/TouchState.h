#pragma once

//Core.
#include <Core/Core/CatalystCore.h>

//Input.
#include <Input/InputCore.h>

class TouchState final
{

public:

	//The button state.
	ButtonState _ButtonState{ ButtonState::ReleasedHold };

	//The X axis value.
	float _X{ 0.0f };

	//The Y axis value.
	float _Y{ 0.0f };

};