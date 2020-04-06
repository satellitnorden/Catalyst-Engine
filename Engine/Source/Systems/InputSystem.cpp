//Header file.
#include <Systems/InputSystem.h>

//Systems.
#include <Systems/CatalystEngineSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Initializes the input system.
*/
void InputSystem::Initialize() NOEXCEPT
{
	//Register the update.
	CatalystEngineSystem::Instance->RegisterUpdate([](void* const RESTRICT arguments)
	{
		static_cast<InputSystem *const RESTRICT>(arguments)->InputUpdate();
	},
	this,
	UpdatePhase::INPUT,
	UpdatePhase::USER_INTERFACE);
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

/*
*	Updates the input system during the input update phase.
*/
void InputSystem::InputUpdate() NOEXCEPT
{
	//Retrieve the current gamepad states.
	for (uint8 i{ 0 }; i < InputConstants::MAXIMUM_NUMBER_OF_GAMEPADS; ++i)
	{
		CatalystPlatform::GetCurrentGamepadState(i, &_InputState._GamepadStates[i]);
	}

	//Retrieve the current keyboard state.
	CatalystPlatform::GetCurrentKeyboardState(&_InputState._KeyboardState);

	//Retrieve the current mouse state.
	CatalystPlatform::GetCurrentMouseState(&_InputState._MouseState);
}