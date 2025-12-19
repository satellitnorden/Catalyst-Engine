//Header file.
#include <UI/Core/ScrollableInterface.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	ScrollableInterface::ScrollableInterface() NOEXCEPT
	{
		//Set the '_IsInside' function.
		_IsInside = [](const UI::Widget *const RESTRICT widget, const Vector2<float32> position)
		{
			return widget->GetAxisAlignedBoundingBox().IsInside(position);
		};
	}

}