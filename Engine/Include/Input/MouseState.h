#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/InputCore.h>

class MouseState final
{

public:

	//The current X.
	float _CurrentX{ 0.0f };

	//The current Y.
	float _CurrentY{ 0.0f };

	//The previous X.
	float _PreviousX{ 0.0f };

	//The previous Y.
	float _PreviousY{ 0.0f };

	//The delta X.
	float _DeltaX{ 0.0f };

	//The delta Y.
	float _DeltaY{ 0.0f };

	//The left button state.
	ButtonState _Left{ ButtonState::RELEASED_HELD };

	//The scroll wheel button state.
	ButtonState _ScrollWheel{ ButtonState::RELEASED_HELD };

	//The right button state.
	ButtonState _Right{ ButtonState::RELEASED_HELD };

	//The scroll wheel step.
	int8 _ScrollWheelStep{ 0 };

};