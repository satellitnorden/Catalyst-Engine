#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Background widget.
	*	Simple widget that covers the container with a solid color.
	*/
	class BackgroundWidget final : public UI::Widget
	{

	public:

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
		FORCE_INLINE BackgroundWidget *const RESTRICT SetColor(const Vector4<float32> &value) NOEXCEPT
		{
			_Color = value;
			return this;
		}

	private:

		//The color.
		Vector4<float32> _Color{ 0.5f, 0.5f, 0.5f, 0.5f };

	};

}