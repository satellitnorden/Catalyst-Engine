#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/Identifier.h>
#include <UI/Core/Container.h>

namespace UI
{

	/*
	*	Base class for all widgets.
	*/
	class Widget
	{

	public:

		/*
		*	Default constructor.
		*/
		FORCE_INLINE Widget(UI::Container *const RESTRICT parent) NOEXCEPT
			:
			_Parent(parent)
		{

		}

		/*
		*	Default destructor.
		*/
		FORCE_INLINE virtual ~Widget() NOEXCEPT
		{

		}

	protected:

		//The parent.
		UI::Container *RESTRICT _Parent;

		//The axis aligned bounding box.
		AxisAlignedBoundingBox2D _AxisAlignedBoundingBox;

	};

}