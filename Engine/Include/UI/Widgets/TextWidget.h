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
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

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
		*	Sets the color.
		*/
		FORCE_INLINE TextWidget *const RESTRICT SetColor(const Vector3<float32> &color) NOEXCEPT
		{
			_ColorOpacity._R = color._R;
			_ColorOpacity._G = color._G;
			_ColorOpacity._B = color._B;
			return this;
		}

		/*
		*	Sets the opacity.
		*/
		FORCE_INLINE TextWidget *const RESTRICT SetOpacity(const float32 opacity) NOEXCEPT
		{
			_ColorOpacity._A = opacity;
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

		//The color/opacity.
		Vector4<float32> _ColorOpacity{ 1.0f, 1.0f, 1.0f, 1.0f };

	};

}