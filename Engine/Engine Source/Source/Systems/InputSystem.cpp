//Header file.
#include <Systems/InputSystem.h>

//Input.
#include <Input/InputUtilities.h>

//Systems.
#include <Systems/QuestSystem.h>

//Singleton definition.
DEFINE_SINGLETON(InputSystem);

/*
*	Default constructor.
*/
InputSystem::InputSystem() NOEXCEPT
{

}

/*
*	Default destructor.
*/
InputSystem::~InputSystem() NOEXCEPT
{

}

/*
*	Post-initializes the input system.
*/
void InputSystem::PostInitializeSystem() NOEXCEPT
{
	//Register the input system asynchronous update daily quest.
	QuestSystem::Instance->RegisterDailyQuest(DailyQuests::InputSystemAsynchronousUpdate, [](void *RESTRICT arguments) { StaticCast<InputSystem *RESTRICT>(arguments)->UpdateSystemAsynchronous(); });
}

/*
*	Pre-updates the input system synchronously.
*/
void InputSystem::PreUpdateSystemSynchronous() NOEXCEPT
{
	//Carry out the input system asynchronous update daily quest.
	QuestSystem::Instance->CarryOutDailyQuest(DailyQuests::InputSystemAsynchronousUpdate, this);
}

/*
*	Post-updates the input system synchronously.
*/
void InputSystem::PostUpdateSystemSynchronous() NOEXCEPT
{
	//Wait for the input system asynchronous update daily quest to finish.
	QuestSystem::Instance->WaitForDailyQuest(DailyQuests::InputSystemAsynchronousUpdate);
}

/*
*	Updates the input system asynchronously.
*/
void InputSystem::UpdateSystemAsynchronous() NOEXCEPT
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
void InputSystem::ReleaseSystem() NOEXCEPT
{
	
}