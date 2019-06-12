#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/General/UpdateContext.h>

//Multithreading.
#include <Multithreading/Task.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>
#include <Input/TouchState.h>

class InputSystem final
{

public:

	/*
	*	Updates the input system during the pre update phase.
	*/
	static void PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT;

};