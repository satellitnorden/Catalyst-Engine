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

};