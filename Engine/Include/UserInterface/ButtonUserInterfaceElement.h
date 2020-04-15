#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class ButtonUserInterfaceElement final : public UserInterfaceElement
{

public:

	//Enumeration covering all states.
	enum class State : uint8
	{
		IDLE,
		HOVERED,
		PRESSED
	};

	//The current state.
	State _CurrentState;

	//The start hovered callback.
	ButtonUserInterfaceElementCallback _StartHoveredCallback;

	//The stop hovered callback.
	ButtonUserInterfaceElementCallback _StopHoveredCallback;

	//The start pressed callback.
	ButtonUserInterfaceElementCallback _StartPressedCallback;

	//The stop pressed callback.
	ButtonUserInterfaceElementCallback _StopPressedCallback;

	//The idle material.
	UserInterfaceMaterial _IdleMaterial;

	//The hovered material.
	UserInterfaceMaterial _HoveredMaterial;

	//The pressed material.
	UserInterfaceMaterial _PressedMaterial;

	//Denotes if this button user interface element is gamepad selected.
	bool _IsGamepadSelected{ false };

};