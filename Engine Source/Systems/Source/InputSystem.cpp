//Header file.
#include <InputSystem.h>

//Input.
#include <InputUtilities.h>

//Systems.
#include <QuestSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Default constructor.
*/
InputSystem::InputSystem() CATALYST_NOEXCEPT
{

}

/*
*	Default destructor.
*/
InputSystem::~InputSystem() CATALYST_NOEXCEPT
{

}

/*
*	Post-initializes the input system.
*/
void InputSystem::PostInitializeSystem() CATALYST_NOEXCEPT
{
	//Register the input system asynchronous update daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::InputSystemAsynchronousUpdate, [](void *CATALYST_RESTRICT arguments) { static_cast<InputSystem *CATALYST_RESTRICT>(arguments)->UpdateSystemAsynchronous(); });
}

/*
*	Pre-updates the input system synchronously.
*/
void InputSystem::PreUpdateSystemSynchronous() CATALYST_NOEXCEPT
{
	//Carry out the input system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::InputSystemAsynchronousUpdate, this);
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() CATALYST_NOEXCEPT
{
	//Update gamepad states.
	for (uint8 i = 0; i < INPUT_MAXIMUM_GAMEPADS; ++i)
	{
		GamepadState currentGamepadStateCopy{ currentGamepadState[i].GetSafe() };

		InputUtilities::GetCurrentGamepadState(i, currentGamepadStateCopy);

		currentGamepadState[i].Set(currentGamepadStateCopy);
	}

	//Update the keyboard.
	KeyboardState currentKeyboardStateCopy{ currentKeyboardState.GetUnsafe() };

	InputUtilities::GetCurrentKeyboardState(currentKeyboardStateCopy);

	currentKeyboardState.Set(currentKeyboardStateCopy);
}

/*
*	Releases the input system.
*/
void InputSystem::ReleaseSystem() CATALYST_NOEXCEPT
{
	
}