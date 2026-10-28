#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Separator widget.
	*	Adds a simple line between items to visually separate sections of the UI.
	*/
	class SeparatorWidget final : public UI::Widget
	{

	public:

		/*
		*	Callback for when the parent is available.
		*/
		void OnParentAvailable() NOEXCEPT override;

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		void Position() NOEXCEPT override;

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

		/*
		*	Sets the color.
		*/
		FORCE_INLINE NO_DISCARD SeparatorWidget* const RESTRICT SetColor(const Vector4<float32> &value) NOEXCEPT
		{
			_Color = value;

			return this;
		}

	private:

		//The color.
		Vector4<float32> _Color{ 0.0f, 0.0f, 0.0f, 0.0f };

	};

}