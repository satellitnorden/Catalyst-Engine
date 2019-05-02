#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElementDescription.h>

class ButtonUserInterfaceElement final
{

public:

	//Enumeration covering all states.
	enum class State : uint8
	{
		Idle,
		Hovered,
		Clicked
	};

	//The description.
	ButtonUserInterfaceElementDescription _Description;

	//The state.
	State _State;

};