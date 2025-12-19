#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Math.
#include <Math/Geometry/AxisAlignedBoundingBox2D.h>

//UI.
#include <UI/Core/UI.h>
#include <UI/Core/Identifier.h>

namespace UI
{

	/*
	*	Container class.
	*/
	class Container final
	{

	public:

		//Enumeration covering all layouts.
		enum class Layout : uint8
		{
			LEFT_TO_RIGHT,
			RIGHT_TO_LEFT,
			TOP_TO_BOTTOM,
			BOTTOM_TO_TOP
		};

		//Denotes if this container is alive.
		bool _Alive;

		//The parent.
		class Scene *RESTRICT _Parent;

		//The identifier.
		UI::Identifier _Identifier;

		//The anchor.
		UI::Anchor _Anchor;

		//The layout.
		Layout _Layout;

		//The axis aligned bounding box.
		AxisAlignedBoundingBox2D _AxisAlignedBoundingBox;

		//The widget size.
		float32 _WidgetSize;

		//The widget padding.
		float32 _WidgetPadding;

		//The start widget index.
		uint64 _StartWidgetIndex;

		//The end widget index.
		uint64 _EndWidgetIndex;

		//The cursor.
		float32 _Cursor;

		//The scroll offset.
		float32 _ScrollOffset;

		//The maximum scroll offset.
		float32 _MaximumScrollOffset;

		//Denotes whether or not this container wants scroll.
		bool _WantsScroll;

	};

}