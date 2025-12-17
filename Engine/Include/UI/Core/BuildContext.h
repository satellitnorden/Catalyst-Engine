#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//UI.
#include <UI/Core/WidgetAllocator.h>

namespace UI
{

	/*
	*	Build context class.
	*/
	class BuildContext final
	{

	public:

		//The widget allocator.
		WidgetAllocator *RESTRICT _WidgetAllocator;

	};

}