//Header file.
#include <UI/Widgets/TextWidget.h>

//UI.
#include <UI/Core/Scene.h>

namespace UI
{

	/*
	*	Callback for when the parent is available.
	*/
	void TextWidget::OnParentAvailable() NOEXCEPT
	{
		//Grab the text scale from the scene.
		_TextScale = _Parent->_Parent->GetTextScale();
	}

	/*
	*	Renders this widget.
	*/
	void TextWidget::Render(const UI::RenderContext &context) NOEXCEPT
	{
		//Render the text.
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