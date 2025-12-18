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
		FORCE_INLINE TextWidget *const RESTRICT SetText(const char* const RESTRICT text) NOEXCEPT
		{
			_Text = text;
			return this;
		}

		/*
		*	Sets the text scale.
		*/
		FORCE_INLINE TextWidget *const RESTRICT SetTextScale(const float32 value) NOEXCEPT
		{
			_TextScale = value;
			return this;
		}

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

	private:

		//The text.
		DynamicString _Text;

		//The text scale.
		float32 _TextScale;

	};

}