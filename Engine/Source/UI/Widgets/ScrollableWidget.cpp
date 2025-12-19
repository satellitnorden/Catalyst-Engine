//Header file.
#include <UI/Widgets/ScrollableWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void ScrollableWidget::Position() NOEXCEPT
	{
		//Just grab the parent's axis aligned bounding box.
		_AxisAlignedBoundingBox = _Parent->_AxisAlignedBoundingBox;
	}

}