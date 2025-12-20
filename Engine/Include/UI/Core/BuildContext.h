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

		//The delta time.
		float32 _DeltaTime;

		//The widget allocator.
		WidgetAllocator *RESTRICT _WidgetAllocator;

	};

}