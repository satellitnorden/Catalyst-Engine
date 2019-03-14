#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/StaticArray.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/KeyboardState.h>
#include <Input/MouseState.h>
#include <Input/TouchState.h>

class InputComponent final
{

public:

	//The gamepad states.
	StaticArray<GamepadState, InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS> _GamepadStates;

	//The keyboard state.
	KeyboardState _KeyboardState;

	//The mouse state.
	MouseState _MouseState;

	//The touch state.
	TouchState _TouchState;

};

/*
*	Function for specifying during which update phase(s) read access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedReadAccess<InputComponent>() NOEXCEPT
{
	return UpdatePhase::Logic | UpdatePhase::Physics | UpdatePhase::Render | UpdatePhase::Post;
}

/*
*	Function for specifying during which update phase(s) write access is allowed to a specific type.
*/
template <>
static NO_DISCARD UpdatePhase AllowedWriteAccess<InputComponent>() NOEXCEPT
{
	return UpdatePhase::Pre;
}