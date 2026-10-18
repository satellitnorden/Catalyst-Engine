#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Input.
#include <Input/MouseState.h>

class PluginUpdateContext final
{

public:

	//The mouse state.
	const MouseState *RESTRICT _MouseState{ nullptr };

};