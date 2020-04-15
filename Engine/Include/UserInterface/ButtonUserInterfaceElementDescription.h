#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Rendering.
#include <Rendering/Native/UserInterfaceMaterial.h>

//User interface.
#include <UserInterface/UserInterfaceElementDescription.h>

class ButtonUserInterfaceElementDescription final : public UserInterfaceElementDescription
{

public:

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

};