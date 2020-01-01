//Header file.
#include <Systems/InputSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Updates the input system during the pre update phase.
*/
void InputSystem::PreUpdate(const UpdateContext *const RESTRICT context) NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i = 0; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_MouseState);
}

/*
*	Sets the cursor position.
*/
void InputSystem::SetCursorPosition(const Vector2<float>& position) NOEXCEPT
{
	//Update internal state.
	_MouseState._CurrentX = position._X;
	_MouseState._CurrentY = position._Y;

	//Update platform cursor.
	CatalystPlatform::SetCursorPosition(position);
}

/*
*	Hides the cursor.
*/
void InputSystem::HideCursor() const NOEXCEPT
{
	CatalystPlatform::HideCursor();
}

/*
*	Shows the cursor.
*/
void InputSystem::ShowCursor() const NOEXCEPT
{
	CatalystPlatform::ShowCursor();
}