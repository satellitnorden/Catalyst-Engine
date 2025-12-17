//Header file.
#include <UI/Widgets/TextWidget.h>

namespace UI
{

	/*
	*	Default constructor.
	*/
	TextWidget::TextWidget(UI::Container *const RESTRICT parent) NOEXCEPT
		:
		UI::Widget(parent)
	{

	}

	/*
	*	Sets the text.
	*/
	TextWidget *const RESTRICT TextWidget::SetText(const char *const RESTRICT text) NOEXCEPT
	{
		//Set the text.
		_Text = text;

		//Return a pointer to this widget so the user can chain calls together. (:
		return this;
	}

}