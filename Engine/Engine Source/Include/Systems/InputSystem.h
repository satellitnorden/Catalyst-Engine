#pragma once

//Engine core.
#include <Engine Core/EngineCore.h>

//Multithreading.
#include <Multithreading/MultithreadedResource.h>

//Input.
#include <Input/GamepadState.h>
#include <Input/InputConstants.h>
#include <Input/KeyboardState.h>

class InputSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(InputSystem);

	/*
	*	Default constructor.
	*/
	InputSystem() NOEXCEPT;

	/*
	*	Default destructor.
	*/
	~InputSystem() NOEXCEPT;

	/*
	*	Post-initializes the input system.
	*/
	void PostInitializeSystem() NOEXCEPT;

	/*
	*	Pre-updates the input system synchronously.
	*/
	void PreUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Post-updates the input system synchronously.
	*/
	void PostUpdateSystemSynchronous() NOEXCEPT;

	/*
	*	Updates the input system asynchronously.
	*/
	void UpdateSystemAsynchronous() NOEXCEPT;

	/*
	*	Releases the input system.
	*/
	void ReleaseSystem() NOEXCEPT;

	/*
	*	Returns the current gamepad state.
	*/
	GamepadState GetCurrentGamepadState(uint8 index = 0) const NOEXCEPT { return currentGamepadState[index].GetSafe(); }

private:

	//The current gamepad states.
	StaticArray<MultithreadedResource<GamepadState>, INPUT_MAXIMUM_GAMEPADS> currentGamepadState;

	//The current keyboard state.
	MultithreadedResource<KeyboardState> currentKeyboardState;

	//Defines whether or not to update buttons.
	std::atomic<bool> updateButtons{ false };

};