#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Scroll bar widget.
	*/
	class ScrollBarWidget final : public UI::Widget
	{

	public:

		//Constants.
		constexpr static float32 SIZE{ 8.0f };

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		void Position() NOEXCEPT override;

		/*
		*	Renders this widget.
		*/
		void Render(const UI::RenderContext &context) NOEXCEPT override;

		/*
		*	Sets the range (in normalized [0.0f, 1.0f] coordinates).
		*/
		FORCE_INLINE ScrollBarWidget *const RESTRICT SetRange(const float32 start, const float32 end) NOEXCEPT
		{
			_Start = start;
			_End = end;
			return this;
		}

	private:

		//The start.
		float32 _Start;

		//The end.
		float32 _End;

	};

}