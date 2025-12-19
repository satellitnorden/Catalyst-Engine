#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//UI.
#include <UI/Core/RenderCommand.h>

namespace UI
{

	/*
	*	Render context class.
	*/
	class RenderContext final
	{

	public:

		//The delta time.
		float32 _DeltaTime;

		//The render commands.
		DynamicArray<RenderCommand> *RESTRICT _RenderCommands;

	};

}