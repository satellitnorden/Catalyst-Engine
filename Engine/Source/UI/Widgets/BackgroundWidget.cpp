//Header file.
#include <UI/Widgets/BackgroundWidget.h>

namespace UI
{

	/*
	*	Positions this widget. Usually called right after creation.
	*/
	void BackgroundWidget::Position() NOEXCEPT
	{
		//If the parent has a next widget bounding box, just use that.
		if (_Parent->_NextWidgetBoundingBox.Valid())
		{
			_AxisAlignedBoundingBox = _Parent->_NextWidgetBoundingBox.Get();
			_Parent->_NextWidgetBoundingBox.Invalidate();

			return;
		}

		//Just grab the parent axis aligned bounding box.
		_AxisAlignedBoundingBox = _Parent->_AxisAlignedBoundingBox;
	}

	/*
	*	Renders this widget.
	*/
	void BackgroundWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the box.
		RenderSolidBox(context, _AxisAlignedBoundingBox, _ColorOrTexture, _ColorOpacity);
	}

}