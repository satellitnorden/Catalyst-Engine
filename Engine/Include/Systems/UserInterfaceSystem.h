#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/ButtonUserInterfaceElement.h>

class UserInterfaceSystem final
{

public:

	/*
	*	Updates the user interface system.
	*/
	static void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Creates a button user interface element.
	*/
	RESTRICTED static NO_DISCARD ButtonUserInterfaceElement *const RESTRICT CreateButtonUserInterfaceElement(const ButtonUserInterfaceElementDescription &description) NOEXCEPT;

};