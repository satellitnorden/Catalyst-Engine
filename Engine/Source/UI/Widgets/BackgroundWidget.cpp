//Header file.
#include <UI/Widgets/BackgroundWidget.h>

namespace UI
{

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void BackgroundWidget::Position() NOEXCEPT
	{
		//Just grab the parent axis aligned bounding box.
		_AxisAlignedBoundingBox = _Parent->_AxisAlignedBoundingBox;
	}

	/*
	*	Renders this widget.
	*/
	void BackgroundWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the box.
		RenderBox(context, _AxisAlignedBoundingBox, _ColorOrTexture, _ColorOpacity);
	}

}