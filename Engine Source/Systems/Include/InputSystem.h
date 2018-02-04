#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <MultithreadedResource.h>

//Input.
#include <GamepadState.h>
#include <InputConstants.h>
#include <KeyboardState.h>

class InputSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InputSystem);

	/*
	*	Default constructor.
	*/
	InputSystem() CATALYST_NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~InputSystem() CATALYST_NOEXCEPT;

	/*
	*	Post-initializes the input system.
	*/
	void PostInitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Pre-updates the input system synchronously.
	*/
	void PreUpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Post-updates the input system synchronously.
	*/
	void PostUpdateSystemSynchronous() CATALYST_NOEXCEPT;

	/*
	*	Updates the input system asynchronously.
	*/
	void UpdateSystemAsynchronous() CATALYST_NOEXCEPT;

	/*
	*	Releases the input system.
	*/
	void ReleaseSystem() CATALYST_NOEXCEPT;

	/*
	*	Returns the current gamepad state.
	*/
	GamepadState GetCurrentGamepadState(uint8 index = 0) const CATALYST_NOEXCEPT { return currentGamepadState[index].GetSafe(); }

private:

	//The current gamepad states.
	StaticArray<MultithreadedResource<GamepadState>, INPUT_MAXIMUM_GAMEPADS> currentGamepadState;

	//The current keyboard state.
	MultithreadedResource<KeyboardState> currentKeyboardState;

	//Defines whether or not to update buttons.
	std::atomic<bool> updateButtons{ false };

};