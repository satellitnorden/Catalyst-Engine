#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>
#include <Core/Containers/DynamicArray.h>

//User interface.
#include <UserInterface/UserInterfaceElement.h>
#include <UserInterface/UserInterfaceElementDescription.h>

class UserInterfaceSystem final
{

public:

	//Singleton declaration.
	DECLARE_SINGLETON(UserInterfaceSystem);

	/*
	*	Default constructor.
	*/
	FORCE_INLINE UserInterfaceSystem() NOEXCEPT
	{

	}

	/*
	*	Initializes the user interface system.
	*/
	void Initialize() NOEXCEPT;

	/*
	*	Terminates the user interface system.
	*/
	void Terminate() NOEXCEPT;

	/*
	*	Creates a user interface element.
	*/
	RESTRICTED NO_DISCARD UserInterfaceElement *const RESTRICT CreateUserInterfaceElement(const UserInterfaceElementDescription *const RESTRICT description) NOEXCEPT;

	/*
	*	Destroys a user interface element.
	*/
	void DestroyUserInterfaceElement(UserInterfaceElement *const RESTRICT element) NOEXCEPT;

	/*
	*	Returns the user interface elements.
	*/
	FORCE_INLINE RESTRICTED NO_DISCARD const DynamicArray<UserInterfaceElement *RESTRICT> *const RESTRICT GetUserInterfaceElements() const NOEXCEPT
	{
		return &_UserInterfaceElements;
	}

private:

	//Container for all user interface elements.
	DynamicArray<UserInterfaceElement *RESTRICT> _UserInterfaceElements;

	/*
	*	Updates the user interface system during the user interface update phase.
	*/
	void UserInterfaceUpdate() NOEXCEPT;

};