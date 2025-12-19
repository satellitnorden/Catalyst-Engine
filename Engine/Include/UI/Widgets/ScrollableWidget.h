#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Widget.h>

namespace UI
{

	/*
	*	Scrollable widget, basically empty with a scrollable interface.
	*	Can be used to hook of scrollable regions.
	*	Will cover the entire container.
	*/
	class ScrollableWidget final : public UI::Widget
	{

	public:

		/*
		*	Positions this widget. Usually called right after creation.
		*/
		void Position() NOEXCEPT override;

		/*
		*	Returns the scrollable interface (if this widget is scrollable).
		*/
		FORCE_INLINE NO_DISCARD UI::ScrollableInterface *const RESTRICT GetScrollableInterface() NOEXCEPT override
		{
			return &_ScrollableInterface;
		}

	private:

		//The scrollable interface.
		UI::ScrollableInterface _ScrollableInterface;

	};

}