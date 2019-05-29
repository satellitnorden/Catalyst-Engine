#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>
#include <UserInterface/UserInterfaceElementDescription.h>

class UserInterfaceSystem final
{

public:

	/*
	*	Updates the user interface system.
	*/
	static void Update(const UpdateContext *const RESTRICT context) NOEXCEPT;

	/*
	*	Terminates the user interface system.
	*/
	static void Terminate() NOEXCEPT;

	/*
	*	Creates a user interface element.
	*/
	RESTRICTED static NO_DISCARD UserInterfaceElement *const RESTRICT CreateUserInterfaceElement(const UserInterfaceElementDescription *const RESTRICT description) NOEXCEPT;

	/*
	*	Destroys a user interface element.
	*/
	static void DestroyUserInterfaceElement(UserInterfaceElement *const RESTRICT element) NOEXCEPT;

};