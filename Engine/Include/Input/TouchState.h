#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Input.
#include <Input/InputCore.h>

class TouchState final
{

public:

	/*
	*	Finger state class definition.
	*/
	class FingerState final
	{

	public:

		//The button state.
		ButtonState _ButtonState{ ButtonState::RELEASED_HELD };

		//The current X.
		float32 _CurrentX{ 0.0f };

		//The current Y.
		float32 _CurrentY{ 0.0f };

		//The previous X.
		float32 _PreviousX{ 0.0f };

		//The previous Y.
		float32 _PreviousY{ 0.0f };

		//The delta X.
		float32 _DeltaX{ 0.0f };

		//The delta Y.
		float32 _DeltaY{ 0.0f };

	};

	//The finger states.
	StaticArray<FingerState, InputConstants::MAXIMUM_NUMBER_OF_TOUCH_FINGERS> _FingerStates;

};