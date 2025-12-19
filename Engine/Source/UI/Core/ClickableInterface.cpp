//Header file.
#include <UI/Core/ClickableInterface.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	ClickableInterface::ClickableInterface() NOEXCEPT
	{
		//Set the initial state.
		_State = State::IDLE;

		//Set the '_IsInside' function.
		_IsInside = [](const UI::Widget *const RESTRICT widget, const Vector2<float32> position)
		{
			return widget->GetAxisAlignedBoundingBox().IsInside(position);
		};
	}

}