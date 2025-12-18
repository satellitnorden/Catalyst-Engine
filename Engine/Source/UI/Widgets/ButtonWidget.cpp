//Header file.
#include <UI/Widgets/ButtonWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	ButtonWidget::ButtonWidget(UI::Container *const RESTRICT parent) NOEXCEPT
		:
		UI::Widget(parent)
	{
		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Renders this widget.
	*/
	void ButtonWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the box.
		RenderBox(context, _AxisAlignedBoundingBox);

		//Render the text.
		if (_Text.Length() > 0)
		{
			RenderText
			(
				context,
				_AxisAlignedBoundingBox,
				_Parent->_Parent->GetFont(),
				_Text.Data(),
				_Text.Length(),
				_TextScale,
				UI::HorizontalAlignment::CENTER,
				UI::VerticalAlignment::CENTER
			);
		}
	}

}