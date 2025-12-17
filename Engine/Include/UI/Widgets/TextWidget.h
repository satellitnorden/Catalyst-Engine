#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/DynamicString.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Text widget, just renders strings.
	*/
	class TextWidget final : public UI::Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		TextWidget(UI::Container *const RESTRICT parent) NOEXCEPT;

		/*
		*	Sets the text.
		*/
		TextWidget *const RESTRICT SetText(const char *const RESTRICT text) NOEXCEPT;

	private:

		//The text.
		DynamicString _Text;

	};

}