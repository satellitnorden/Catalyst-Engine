#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>

class ButtonUserInterfaceElement final : public UserInterfaceElement
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

	//The idle texture index.
	uint32 _IdleTextureIndex;

	//The hovered texture index.
	uint32 _HoveredTextureIndex;

	//The pressed texture index.
	uint32 _PressedTextureIndex;

};