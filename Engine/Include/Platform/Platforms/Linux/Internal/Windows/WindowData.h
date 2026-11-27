#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Platform.
#include <Platform/Core/WindowManager.h>
#include <Platform/Platforms/Linux/Internal/Wayland/WaylandWindowData.h>

/*
*	Window data class definition.
*/
class WindowData final
{

public:

	//The parameters.
	Platform::WindowManager::CreateParameters _Parameters;

	//The Wayland window data.
	WaylandWindowData _WaylandWindowData;

	//The final resolution.
	Resolution _FinalResolution;

	//Denotes whether or not this window supports OpenGL.
	bool _SupportsOpenGL{ false };

	//Denotes whether or not this window is focused.
	bool _Focused{ false };

	//The callbacks.
	Platform::WindowManager::Callbacks _Callbacks;

	//The last mouse position.
	Vector2<float32> _LastMousePosition{ 0.5f, 0.5f };

	//The timer identifier generator.
	uint64 _TimerIdentifierGenerator{ 0 };

};