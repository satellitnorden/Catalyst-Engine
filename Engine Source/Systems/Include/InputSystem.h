#pragma once

//Engine core.
#include <EngineCore.h>

//Multithreading.
#include <MultithreadedResource.h>

//Input.
#include <GamepadState.h>

class InputSystem
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
	*	Initializes the input system.
	*/
	void InitializeSystem() CATALYST_NOEXCEPT;

	/*
	*	Updates the input system synchronously.
	*/
	void UpdateSystemSynchronous() CATALYST_NOEXCEPT;

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
	CATALYST_FORCE_INLINE GamepadState GetCurrentGamepadState(uint8 index = 0) const CATALYST_NOEXCEPT { return currentGamepadState[index].GetSafe(); }

private:

	//The current gamepad state.
	MultithreadedResource<GamepadState> currentGamepadState[4];

	//Defines whether or not to update gamepad buttons.
	std::atomic<bool> updateGamepadButtons{ false };

};